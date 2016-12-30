
#include "qmpptdata.h"

/*
TriStar-60 MPPT	Controller v1.1.12
Serial #12230141	Server v5.19
*/

QmpptData::QmpptData(QObject *parent) :
      QObject(parent)
{

}

#define REG_Vb  38      // "V","fD0","Battery Voltage",1);
#define REG_VbT 51      // "V","fD1","Target Voltage",1);
#define REG_IbC 39      // "A","fD2","Charge Current",1);
#define REG_Va  27      // "V","fD9","Array Voltage",1);
#define REG_Ia  29      // "A","fD10","Array Current",1);
#define REG_Ib  58      // "W","fD4","Output Power",1);

#define BTemp   37

#define BLOCK_START     27
#define BLOCK_SIZE      32
#define BLOCK_LENGTH_POS 8

void QmpptData::readData()
{

    socket = new QTcpSocket(this);

    socket->connectToHost("192.168.0.124",502);

    if( !socket->waitForConnected(2000) )
    {
        emit statusBar( "MPPT not Connected" );
        return;
    }

    emit statusBar( "TriStar MPPT Solar Charger, ModBus connected" );

    // == prepare the ModBus block

    QByteArray block;
    block.resize(12);

    // == see http://www.simplymodbus.ca/TCP.htm ************

    block [0] = 0x00;           // unique transaction identifier HI BYTE
    block [1] = 0x01;           // unique transaction identifier LO BYTE
    block [2] = 0x00;           // protocol identifier HI
    block [3] = 0x00;           // protocol identifier LO
    block [4] = 0x00;           // message length i.e. 6 bytes to follow HI
    block [5] = 0x06;           // message length i.e. 6 bytes to follow LO
    block [6] = 0x01;           // unit identifier

    block [7] = 0x04;           // the function code (--> read input registers)

    block [8] = 0x00;           // HI byte
    block [9] = BLOCK_START;    // Start from register mem addr

    block [10] = 0x00;          // Hi byte
    block [11] = BLOCK_SIZE;    // Read in 2 registers

    socket->write( block );
    socket->waitForBytesWritten(1000);

    // ==== read the block ==================

    socket->waitForReadyRead(1000);
    block = socket->readAll();

    socket->close();

    int nBytes = block[ BLOCK_LENGTH_POS ];

    QList<ushort> sdata;          // mppr solar data
    sdata.clear();

    for( int n=0; n<nBytes; n+=2 ) {

        uchar hb = block.at(BLOCK_LENGTH_POS + 1 + n );
        uchar lb = block.at(BLOCK_LENGTH_POS + 1 + n+1 );

        sdata.append((hb<<8) | lb );
    }

    // == calculate MPPT data with scaling

#define VSCALE (float)90
#define ISCALE (float)40

    _hashData.clear();

    float Vb = sdata.at(REG_Vb - BLOCK_START) * VSCALE/(1<<14);
    _hashData.insert("Vb", Vb);

    float Ib = sdata.at(REG_IbC - BLOCK_START) * ISCALE/(1<<14);
    if( Ib>60. ) Ib = 0.;
    _hashData.insert("Ib", Ib);

    float Va = sdata.at(REG_Va - BLOCK_START) * VSCALE/(1<<14);
    _hashData.insert("Va", Va);

    //qDebug() << sdata.at(REG_Ia - BLOCK_START);
    float Ia = sdata.at(REG_Ia - BLOCK_START) * ISCALE/(1<<14);
    if( Ia>60. ) Ia = 0.;
    _hashData.insert("Ia", Ia);

    float Pa = sdata.at(REG_Ib - BLOCK_START) * VSCALE*ISCALE/(1<<15);
    _hashData.insert("Pa", Pa);

    float Tb = sdata.at(BTemp - BLOCK_START);
    _hashData.insert("Tb", Tb);

    emit mpptData(_hashData);
}
