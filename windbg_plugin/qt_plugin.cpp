#include "pch.h"
#include "qt_plugin.h"

#include "inc/wdbgexts.h"
#include <strsafe.h>

// Helper function prototypes
VOID InOrderTraversal(ULONG_PTR pNode);
VOID PrintNode(ULONG ulLeft, ULONG ulData, ULONG ulRight);
HRESULT GetNodeValues(ULONG64 pNode, ULONG* pulData, ULONG* pulLeft,
                      ULONG* pulRight);
VOID ReleaseComPointers();

BOOL bInitialized = FALSE;

// Required COM interfaces
IDebugClient* pDebugClient = NULL;
IDebugControl* pDebugControl = NULL;
IDebugSymbols* pSymbols = NULL;
IDebugSymbols3* pSymbols3 = NULL;
IDebugDataSpaces* pDataSpaces = NULL;

// Globals
ULONG ulLeftOffset = 0;
ULONG ulRightOffset = 0;
ULONG ulDataOffset = 0;
ULONG pSize = NULL;
ULONG pSizeData = NULL;
ULONG pSizeChildPtr = NULL;
ULONG pNodeIndex = NULL;
ULONG64 pBase = NULL;

extern "C" HRESULT CALLBACK DebugExtensionInitialize(PULONG Version,
                                                     PULONG Flags) {
  HRESULT hRes = E_FAIL;

  if (bInitialized) {
    return S_OK;
  }

  //
  // Initialize the version information
  //
  *Version = DEBUG_EXTENSION_VERSION(1, 0);
  *Flags = 0;

  //
  // Initialize required COM interface pointers
  //
  if (FAILED(hRes =
                 DebugCreate(__uuidof(IDebugClient), (void**)&pDebugClient))) {
    return hRes;
  }

  if (FAILED(hRes = pDebugClient->QueryInterface(__uuidof(IDebugControl),
                                                 (void**)&pDebugControl))) {
    ReleaseComPointers();
    return hRes;
  }

  dprintf("Failed to get required COM interface\n");
  //
  // Initialize WinDbg extension data
  //
  ExtensionApis.nSize = sizeof(ExtensionApis);
  hRes = pDebugControl->GetWindbgExtensionApis64(&ExtensionApis);

  if (FAILED((hRes = pDebugClient->QueryInterface(__uuidof(IDebugDataSpaces),
                                                  (void**)&pDataSpaces)))) {
    dprintf("Failed to get required COM interface\n");
    ReleaseComPointers();
    return hRes;
  }

  if (FAILED(hRes = pDebugClient->QueryInterface(__uuidof(IDebugSymbols),
                                                 (void**)&pSymbols))) {
    dprintf("Failed to get required COM interface\n");
    ReleaseComPointers();
    return hRes;
  }

  if (FAILED(hRes = pDebugClient->QueryInterface(__uuidof(IDebugSymbols3),
                                                 (void**)&pSymbols3))) {
    dprintf("Failed to get required COM interface\n");
    ReleaseComPointers();
    return hRes;
  }

    //
    // Initialize type information
    //
    // if(FAILED(hRes=pSymbols->GetModuleByModuleName("Platform", 
    //                                                0, 
    //                                                NULL, 
    //                                                &pBase)))
    // {
    //     dprintf("Failed to get module information for bstree.exe\n");
    //     ReleaseComPointers();        
    //     return hRes;
    // }
//     if(FAILED(hRes=pSymbols3->GetTypeId(pBase, 
//                                         "QString",    
//                                         &pNodeIndex)))
//     {
//         dprintf("Failed to get type id\n");
//         ReleaseComPointers();        
//         return hRes;
//     }

//     if(FAILED(hRes=pSymbols->GetTypeSize(pBase, 
//                                          pNodeIndex, 
//                                          &pSize)))
//     {
//         dprintf("Failed to get type size\n");
//         ReleaseComPointers();        
//         return hRes;
//     }
// else{
//     dprintf("size: %ud", pSize);
// }

  bInitialized = TRUE;
  hRes = S_OK;

  return hRes;
}

extern "C" void CALLBACK DebugExtensionUninitialize(void) {
  ReleaseComPointers();
  return;
}

HRESULT CALLBACK help(PDEBUG_CLIENT Client, PCSTR Args) {
  dprintf(
      "SYSYEXTS.DLL commands:\n"
      "\thelp                  = Shows this help\n"
      "\tdumptree <address>    = Traverses a binary tree. Address must point "
      "to the root node of the tree");

  return S_OK;
}

void dumpString(ULONG_PTR pNode);

HRESULT CALLBACK dumptree(PDEBUG_CLIENT pClient, PCSTR szArgs) {
  HRESULT hRes = S_OK;

  ULONG_PTR pAddress = (ULONG_PTR)GetExpression(szArgs);
  if (!pAddress) {
    dprintf("Invalid head pointer address specified: %s\n", szArgs);
    return E_FAIL;
  }

  // InOrderTraversal(pAddress);
  dumpString(pAddress);
  return hRes;
}

HRESULT CALLBACK dumpQString(PDEBUG_CLIENT pClient, PCSTR szArgs) {
  HRESULT hRes = S_OK;

  ULONG_PTR pAddress = (ULONG_PTR)GetExpression(szArgs);
  if (!pAddress) {
    dprintf("Invalid head pointer address specified: %s\n", szArgs);
    return E_FAIL;
  }

  ULONG dAddress = 0;
  if (FAILED(hRes = pDataSpaces->ReadVirtual((ULONG_PTR)pAddress, &dAddress,
                                             sizeof(ULONG), NULL))) {
    dprintf("Failed to read memory at address: 0x%X\n", pAddress);
    return E_FAIL;
  }

  wchar_t buff[256] = {0};
  if (FAILED(hRes = pDataSpaces->ReadVirtual((ULONG_PTR)dAddress + 16, buff,
                                             256, NULL))) {
    dprintf("Failed to read memory at address: 0x%X\n", pAddress + 16);
  } else {
    dprintf("QString value: %ws\n", buff);
  }

  return hRes;
}

void dumpString(ULONG_PTR pNode) {
  wchar_t buff[256] = {0};
  ULONG ulData = 0;
  HRESULT hRes = S_OK;
  if (FAILED(hRes = pDataSpaces->ReadVirtual((ULONG_PTR)pNode + 16, buff, 256,
                                             NULL))) {
    dprintf("Failed to read memory at address: 0x%X\n", pNode + 16);
  } else {
    dprintf("string: %ws", buff);
  }
}

VOID InOrderTraversal(ULONG_PTR pNode) {
  HRESULT hRes = S_OK;
  PULONG pOffset = NULL;
  BYTE* pBuffer = NULL;
  ULONG ulData = 0;
  ULONG ulLeft = 0;
  ULONG ulRight = 0;

  if (CheckControlC() == TRUE) {
    dprintf("Control C hit, canceling command\n");
    return;
  }

  if (!pNode) {
    return;
  }

  if (FAILED(hRes = GetNodeValues(pNode, &ulData, &ulLeft, &ulRight))) {
    return;
  }

  if (ulLeft) {
    InOrderTraversal(ulLeft);
    delete[] pBuffer;
  }

  PrintNode(ulLeft, ulData, ulRight);

  if (ulRight) {
    InOrderTraversal(ulRight);
    delete[] pBuffer;
  }
}

HRESULT GetNodeValues(ULONG64 pNode, ULONG* pulData, ULONG* pulLeft,
                      ULONG* pulRight) {
  HRESULT hRes = S_OK;
  if (FAILED(hRes = pDataSpaces->ReadVirtual((ULONG_PTR)pNode + ulDataOffset,
                                             pulData, sizeof(ULONG), NULL))) {
    dprintf("Failed to read memory at address: 0x%X\n", pNode + ulDataOffset);
    return E_FAIL;
  }

  if (FAILED(hRes = pDataSpaces->ReadVirtual((ULONG_PTR)pNode + ulLeftOffset,
                                             pulLeft, sizeof(ULONG), NULL))) {
    dprintf("Failed to read memory at address: 0x%X\n", pNode + ulLeftOffset);
    return E_FAIL;
  }

  if (FAILED(hRes = pDataSpaces->ReadVirtual((ULONG_PTR)pNode + ulRightOffset,
                                             pulRight, sizeof(ULONG), NULL))) {
    dprintf("Failed to read memory at address: 0x%X\n", pNode + ulRightOffset);
    return E_FAIL;
  }

  return S_OK;
}

VOID PrintNode(ULONG ulLeft, ULONG ulData, ULONG ulRight) {
  dprintf("** Node **\n");
  dprintf("   Data: %d\n", ulData);
  dprintf("   Left child pointer: 0x%x\n", ulLeft);
  dprintf("   Right child pointer: 0x%x\n", ulRight);
}

// HRESULT CALLBACK KnownStructOutput(IN ULONG Flag, IN ULONG64 Address,
//                                    IN PSTR StructName, OUT PSTR Buffer,
//                                    IN OUT PULONG BufferSize) {
//   HRESULT hRes = E_FAIL;

//   //if (Flag == DEBUG_KNOWN_STRUCT_GET_NAMES) {
//   //  if ((*BufferSize) < strlen(SYSEXTS_KNOWNSTRUCT_1) + 2) {
//   //    // Not enough buffer available, return S_FALSE
//   //    (*BufferSize) = strlen(SYSEXTS_KNOWNSTRUCT_1) + 2;
//   //    hRes = S_FALSE;
//   //  } else {
//   //    hRes = StringCchPrintfA(Buffer, (*BufferSize) - 2, "%s\0",
//   //                            SYSEXTS_KNOWNSTRUCT_1);
//   //    if (FAILED(hRes)) {
//   //      dprintf("Failed to copy the data type name into buffer\n");
//   //    }
//   //  }
//   //} else if (Flag == DEBUG_KNOWN_STRUCT_GET_SINGLE_LINE_OUTPUT) {
//   //  ULONG ulData = 0;
//   //  ULONG ulLeft = 0;
//   //  ULONG ulRight = 0;

//   //  if (FAILED(hRes = GetNodeValues(Address, &ulData, &ulLeft, &ulRight))) {
//   //    return hRes;
//   //  }

//   //  DWORD dwLen =
//   //      _scprintf(SYSEXTS_KNOWNSTRUCT_OUT, ulData, ulLeft, ulRight) + 1;

//   //  if (dwLen > (*BufferSize)) {
//   //    dprintf("KnownStructOutput unable to fit return data into buffer\n");
//   //    hRes = E_FAIL;
//   //    return hRes;
//   //  }

//   //  hRes = StringCchPrintfA(Buffer, dwLen, SYSEXTS_KNOWNSTRUCT_OUT, ulData,
//   //                          ulLeft, ulRight);
//   //  if (FAILED(hRes)) {
//   //    dprintf("KnownStructOutput unable to write data into buffer\n");
//   //  }
//   //} else if (Flag == DEBUG_KNOWN_STRUCT_SUPPRESS_TYPE_NAME) {
//   //  return S_OK;
//   //} else {
//   //  dprintf("KnownStructOutput called with invalid flags\n");
//   //}
//   return hRes;
// }

VOID ReleaseComPointers() {
  if (pDebugClient) pDebugClient->Release();
  if (pDebugControl) pDebugControl->Release();
  if (pSymbols) pSymbols->Release();
  if (pSymbols3) pSymbols3->Release();
  if (pDataSpaces) pDataSpaces->Release();
}

// HRESULT CALLBACK
// DebugExtensionInitialize(PULONG Version, PULONG Flags)
//
//{
//        *Version = DEBUG_EXTENSION_VERSION(1, 0);
//        *Flags = 0;  // ÔÝÊ±²»ÓÃ
//        return S_OK;
// }
//
// HRESULT CALLBACK Test(PDEBUG_CLIENT pDebugClient, PCSTR args)
//
//{
//        //UNREFERENCED_PARAMETER(args);
//        //IDebugControl* pDebugControl;
//
//        //if (SUCCEEDED(pDebugClient->QueryInterface(__uuidof(IDebugControl4),
//        (void **)&pDebugControl)))
//        //{
//
//        //       pDebugControl->Output(DEBUG_OUTPUT_NORMAL, "Hello Nano
//        Code,Here is GDK7!!!\n");
//        //       pDebugControl->Release();
//        //}
//
//
//      static ULONG Address = 0;
//     ULONG  GetAddress, StringAddress, Index = 0, Bytes;
//     WCHAR MyString[51] = {0};
//
//
//     GetAddress = GetExpression(args);
//
//     if(GetAddress != 0)
//     {
//         Address = GetAddress;
//     }
//
//     dprintf("STACK   ADDR   STRING \n");
//
//     for(Index = 0; Index < 4*20; Index+=4)
//     {
//         memset(MyString, 0, sizeof(MyString));
//
//         Bytes = 0;
//
//         ReadMemory(Address + Index, &StringAddress, sizeof(StringAddress),
//         &Bytes);
//
//         if(Bytes)
//         {
//            Bytes = 0;
//
//            ReadMemory(StringAddress, MyString, sizeof(MyString) - 2, &Bytes);
//
//            if(Bytes)
//            {
//               dprintf("%08x : %08x = (UNICODE) \"%ws\"\n", Address + Index,
//               StringAddress, MyString); dprintf("%08x : %08x = (ANSI)
//               \"%s\"\n", Address + Index, StringAddress, MyString);
//            }
//            else
//            {
//               dprintf("%08x : %08x =  Address Not Valid\n", Address + Index,
//               StringAddress);
//            }
//         }
//         else
//         {
//            dprintf("%08x : Address Not Valid\n", Address + Index);
//         }
//     }
//
//     Address += Index;
//        return S_OK;
// }
