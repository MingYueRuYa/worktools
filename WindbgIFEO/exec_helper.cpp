#include "exec_helper.h"
#include <Windows.h>
#include <QFile>
#include <QDebug>


ExecHelper::Architecture ExecHelper::detect_arch(const QString& filePath) {
  QFile file(filePath);
  if (!file.open(QIODevice::ReadOnly)) {
    qDebug() << "Could not open file:" << filePath;
    return Architecture::ARCH_UNKNOWN;
  }

  // Read DOS header (IMAGE_DOS_HEADER)
  IMAGE_DOS_HEADER dosHeader;
  if (file.read(reinterpret_cast<char*>(&dosHeader),
                sizeof(IMAGE_DOS_HEADER)) != sizeof(IMAGE_DOS_HEADER)) {
    qDebug() << "Failed to read DOS header";
    return Architecture::ARCH_UNKNOWN;
  }

  // Check DOS signature
  if (dosHeader.e_magic != IMAGE_DOS_SIGNATURE) {
    qDebug() << "Not a valid DOS header";
    return Architecture::ARCH_UNKNOWN;
  }

  // Move to PE header and read it
  file.seek(dosHeader.e_lfanew);
  DWORD peSignature;
  if (file.read(reinterpret_cast<char*>(&peSignature), sizeof(DWORD)) !=
      sizeof(DWORD)) {
    qDebug() << "Failed to read PE signature";
    return Architecture::ARCH_UNKNOWN;
  }

  // Check PE signature
  if (peSignature != IMAGE_NT_SIGNATURE) {
    qDebug() << "Not a valid PE header";
    return Architecture::ARCH_UNKNOWN;
  }

  // Read File header (IMAGE_FILE_HEADER)
  IMAGE_FILE_HEADER fileHeader;
  if (file.read(reinterpret_cast<char*>(&fileHeader),
                sizeof(IMAGE_FILE_HEADER)) != sizeof(IMAGE_FILE_HEADER)) {
    qDebug() << "Failed to read file header";
    return Architecture::ARCH_UNKNOWN;
  }

  // Determine architecture based on Machine field
  switch (fileHeader.Machine) {
    case IMAGE_FILE_MACHINE_I386:
      return Architecture::ARCH_X86;
    case IMAGE_FILE_MACHINE_AMD64:
      return Architecture::ARCH_X64;
    default:
      return Architecture::ARCH_UNKNOWN;
  }
}
