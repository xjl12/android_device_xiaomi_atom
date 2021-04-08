package org.ifaa.android.manager;

interface IIFAAService {
    byte[] processCmd_v2(in byte[] param);
    int[] getIDList(int bioType);
    int faceEnroll(String sessionId, int flags);
    int faceUpgrade(int action, String path, int offset, in byte[] data, int data_len);
    int faceAuthenticate_v2(String sessionId, int flags);
    int faceCancel_v2(String sessionId);
    byte[] faceInvokeCommand(in byte[] param);
    int faceGetCellinfo();
}
