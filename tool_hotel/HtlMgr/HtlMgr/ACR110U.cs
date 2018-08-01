//  Copyright(C):      Advanced Card Systems Ltd
//
//  File:              ACR110U.cs
//
//  Description:       Header File for ACR110 USB Contactless Reader.
//
//  Author:	           Jose Isagani R. Mission
//
//  Date:	           August 28, 2006
//
//  Revision Trail:   (Date/Author/Description)
//
//                     August 4, 2007/ Aileen Grace L. Sarte/ Change pValueData and pNewValue datatype ref long to ref int 
//                                                             of ACR110_ReadValue and ACR110_Inc, ACR110_Dec, ACR110_Copy;   
//                                                            Change ValueData and value datatype long to int.
//
//========================================================================================================

using System;
using System.Text;
using System.Runtime.InteropServices;



public class ACR110U
{
    //=============================== Error Code ===============================

    public const int ERR_ACR110_INTERNAL_UNEXPECTED = -1000;
    public const int ERR_ACR110_PORT_INVALID = -2000;
    public const int ERR_ACR110_PORT_OCCUPIED = -2010;
    public const int ERR_ACR110_HANDLE_INVALID = -2020;
    public const int ERR_ACR110_INCORRECT_PARAM = -2030;
    public const int ERR_ACR110_READER_NO_TAG = -3000;
    public const int ERR_ACR110_READER_READ_FAIL_AFTER_OP = -3010;
    public const int ERR_ACR110_READER_NO_VALUE_BLOCK = -3020;
    public const int ERR_ACR110_READER_OP_FAILURE = -3030;
    public const int ERR_ACR110_READER_UNKNOWN = -3040;
    public const int ERR_ACR110_READER_LOGIN_INVALID_STORED_KEY_FORMAT = -4010;
    public const int ERR_ACR110_READER_WRITE_READ_AFTER_WRITE_ERROR = -4020;
    public const int ERR_ACR110_READER_DEC_FAILURE_EMPTY = -4030;
    public const int ERR_READER_VALUE_INC_OVERFLOW = -4031;
    public const int ERR_READER_VALUE_OP_FAILURE = -4032;
    public const int ERR_READER_VALUE_INVALID_BLOCK = -4033;
    public const int ERR_READER_VALUE_ACCESS_FAILURE = -4034;
    public const int ERR_READER_NO_RESPONSE = -5000;

    //======================= Reader Port for AC_Open ==========================

    public const int ACR110_USB1 = 0;
    public const int ACR110_USB2 = 1;
    public const int ACR110_USB3 = 2;
    public const int ACR110_USB4 = 3;
    public const int ACR110_USB5 = 4;
    public const int ACR110_USB6 = 5;
    public const int ACR110_USB7 = 6;
    public const int ACR110_USB8 = 7;

    //======================== Key Type for AC_Login ===========================

    public const int ACR110_LOGIN_KEYTYPE_A = (int)(0xAA);
    public const int ACR110_LOGIN_KEYTYPE_B = (int)(0xBB);
    public const int ACR110_LOGIN_KEYTYPE_DEFAULT_A = (int)(0xAD);
    public const int ACR110_LOGIN_KEYTYPE_DEFAULT_B = (int)(0xBD);
    public const int ACR110_LOGIN_KEYTYPE_DEFAULT_F = (int)(0xFD);
    public const int ACR110_LOGIN_KEYTYPE_STORED_A = (int)(0xAF);
    public const int ACR110_LOGIN_KEYTYPE_STORED_B = (int)(0xBF);

    [StructLayout(LayoutKind.Sequential)]
    public struct tReaderStatus
    {
        // 0x01 = Type A; 0x02 = Type B; 0x03 = Type A + Type B
        public byte MifareInterfaceType;

        // Bit 0 = Mifare Light; Bit 1 = Mifare1K; Bit 2 = Mifare 4K; Bit 3 = Mifare DESFire
        // Bit 4 = Mifare UltraLight; Bit 5 = JCOP30; Bit 6 = Shanghai Transport
        // Bit 7 = MPCOS Combi; Bit 8 = ISO type B, Calypso
        // Bit 9 - Bit 31 = To be defined
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 4)]
        public byte[] CardsSupported;

        // 0x00 = Type A; 0x01 = Type B TAG is being processed
        // 0xFF = No TAG is being processed
        public byte CardOpMode;

        public byte FWI;		// the current FWI value (time out value)
        public byte RFU;		// To be defined
        public int RFU2;		// To be defined
    };

    //------------------------------------------------------------------------------------------
    //Prototype section
    //------------------------------------------------------------------------------------------
    //======================================================================================
    // READER COMMANDS
    //======================================================================================


    [DllImport("ACR110U.DLL")]
    public static extern int ACR110_Open(int ReaderPort);

    [DllImport("ACR110U.DLL")]
    public static extern short ACR110_Close(int hReader);

    [DllImport("ACR110U.DLL")]
    public static extern short ACR110_Reset(int hReader);

    [DllImport("ACR110U.DLL")]
    public static extern int ACR110_Status(int hReader, ref byte pFirmwareVersion, ref tReaderStatus pReaderStatus);

    [DllImport("ACR110U.DLL")]
    public static extern int ACR110_ReadRC531Reg(int hReader, byte RegNo, ref byte pRegData);

    [DllImport("ACR110U.DLL")]
    public static extern int ACR110_WriteRC531Reg(int hReader, byte RegNo, byte pRegData);

    [DllImport("ACR110U.DLL")]
    public static extern int ACR110_DirectSend(int hReader, byte DataLength, ref byte pData,
                                               ref byte pResponseDataLength, ref byte pResponseData, int TimedOut);

    [DllImport("ACR110U.DLL")]
    public static extern int ACR110_DirectReceive(int hReader, byte RespectedDataLength, ref byte pReceivedDataLength,
                                                  ref byte pReceivedData, int TimedOut);

    [DllImport("ACR110U.DLL")]
    public static extern int ACR110_RequestDLLVersion(ref byte pVersionInfoLen, ref byte pVersionInfo);

    [DllImport("ACR110U.DLL")]
    public static extern int ACR110_ReadEEPROM(int hReader, byte RegNo, ref byte pEEPROMData);

    [DllImport("ACR110U.DLL")]
    public static extern int ACR110_WriteEEPROM(int hReader, byte RegNo, byte eePROMData);

    [DllImport("ACR110U.DLL")]
    public static extern int ACR110_ReadUserPort(int hReader, ref byte pUserPortState);

    [DllImport("ACR110U.DLL")]
    public static extern int ACR110_WriteUserPort(int hReader, byte userPortState);

    //======================================================================================
    // CARD COMMANDS
    //======================================================================================

    [DllImport("ACR110U.DLL")]
    public static extern short ACR110_Select(int hReader, ref byte pResultTagType,
                                           ref byte pResultTagLength, ref byte pResultSN);

    [DllImport("ACR110U.DLL")]
    public static extern short ACR110_Login(int hReader, byte Sector, int keyType,
                                          byte storedNo, ref byte pKey);

    [DllImport("ACR110U.DLL")]
    public static extern short ACR110_Read(int hReader, byte Block, ref byte pBlockData);

    [DllImport("ACR110U.DLL")]
    public static extern int ACR110_ReadValue(int hReader, byte Block, ref int pValueData);

    [DllImport("ACR110U.DLL")]
    public static extern short ACR110_Write(int hReader, byte Block, ref byte pBlockData);

    [DllImport("ACR110U.DLL", CharSet = CharSet.Auto)]
    public static extern int ACR110_WriteValue(int hReader, byte Block, int ValueData);

    [DllImport("ACR110U.DLL")]
    public static extern int ACR110_WriteMasterKey(int hReader, byte keyNo, ref byte pKey);

    [DllImport("ACR110U.DLL")]
    public static extern int ACR110_Inc(int hReader, byte Block,
                                        int value, ref int pNewValue);

    [DllImport("ACR110U.DLL")]
    public static extern int ACR110_Dec(int hReader, byte Block,
                                        int value, ref int pNewValue);

    [DllImport("ACR110U.DLL")]
    public static extern int ACR110_Copy(int hReader, byte srcBlock,
                                         byte desBlock, ref int pNewValue);

    [DllImport("ACR110U.DLL")]
    public static extern int ACR110_Power(int hReader, byte State);

    [DllImport("ACR110U.DLL")]
    public static extern int ACR110_ListTags(int hReader, ref byte pNumTagFound, ref byte pTagType,
                                             ref byte pTagLength, ref byte pSN);

    [DllImport("ACR110U.DLL")]
    public static extern int ACR110_MultiTagSelect(int hReader, byte TagLength, ref byte SN,
                                                   ref byte pResultTagType, ref byte pResultTagLength,
                                                   ref byte pResultSN);

    [DllImport("ACR110U.DLL")]
    public static extern int ACR110_TxDataTelegram(int hReader, byte SendDataLength, ref byte pSendData,
                                                   ref byte pReceivedDataLength, ref byte pReceivedData);

    // Error routines
    public static string GetErrMsg(int code)
    {
        switch (code)
        {
            case -1000:
                return ("Unexpected Internal Library Error : -1000");

            case -2000:
                return ("Invalid Port : -2000");

            case -2010:
                return ("Port Occupied by Another Application : -2010");

            case -2020:
                return ("Invalid Handle : -2020");

            case -2030:
                return ("Incorrect Parameter : -2030");

            case -3000:
                return ("No TAG Selected or in Reachable Range : -3000");

            case -3010:
                return ("Read Failed after Operation : -3010");

            case -3020:
                return ("Block doesn't contain value : -3020");

            case -3030:
                return ("Operation Failed : -3030");

            case -3040:
                return ("Unknown Reader Error : -3040");

            case -4010:
                return ("Invalid stored key format in login process : -4010");

            case -4020:
                return ("Reader can't read after write operation : -4020");

            case -4030:
                return ("Decrement Failure (Empty) : -4030");

        }
        return ("Error is not documented.");
    }

    public static string GetTagType1(byte XTag)
    {
        switch (Convert.ToInt16(XTag))
        {
            case 1:
                return ("Mifare Light");

            case 2:
                return ("Mifare 1K");

            case 3:
                return ("Mifare 4K");

            case 4:
                return ("Mifare DESFire");

            case 5:
                return ("Mifare Ultralight");

            case 6:
                return ("JCOP30");

            case 7:
                return ("Shanghai Transport");

            case 8:
                return ("MPCOS Combi");

            case 128:
                return ("ISO Type B, Calypso");
        }
        return ("Unidentified Card");

    }

    public ACR110U()
    {
        //
        // TODO: Add constructor logic here
        //
    }
}
