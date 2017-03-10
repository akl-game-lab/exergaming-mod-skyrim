#pragma once

void SafeWrite8(UInt32 addr, UInt32 content);
void SafeWrite16(UInt32 addr, UInt32 content);
void SafeWrite32(UInt32 addr, UInt32 content);
void SafeWriteBuf(UInt32 addr, void * content, UInt32 len);

// 5 bytes
void WriteRelJump(UInt32 jumpSrc, UInt32 jumpTgt);
void WriteRelCall(UInt32 jumpSrc, UInt32 jumpTgt);

// 6 bytes
void WriteRelJnz(UInt32 jumpSrc, UInt32 jumpTgt);
void WriteRelJle(UInt32 jumpSrc, UInt32 jumpTgt);
