/*
   QSPI as USB Mass Storage
   This example shows how to expose a QSPIF BlockDevice (16MB external flash on the Portenta H7)
   as an USB stick. It can be adapted to any kind of BlockDevice (FlashIAP or either RAM via HeapBlockDevice)
   Before loading this example, make sure you execute WiFiFirmwareUpdater sketch
   to create and format the proper partitions.
*/

#include "PluggableUSBMSD.h"
#include "QSPIFBlockDevice.h"
#include "MBRBlockDevice.h"
#include "FATFileSystem.h"

static QSPIFBlockDevice root;
mbed::MBRBlockDevice wifi_data(&root, 1);
mbed::MBRBlockDevice ota_data(&root, 2);
static mbed::FATFileSystem wifi("wifi");
static mbed::FATFileSystem ota("ota");

void USBMSD::begin()
{
  int err = wifi.mount(&wifi_data);
  if (err) {
    while (!Serial);
    Serial.println("Please run WiFiFirmwareUpdater before");
    return;
  }
  ota.mount(&ota_data);
}


USBMSD MassStorage(&root);

void setup() {
  Serial.begin(115200);
  MassStorage.begin();
  char myFileName[] = "ota/info.txt"; 
FILE *myFile = fopen(myFileName, "w");
fprintf(myFile,"Test file, to see how cool this is \r\n");
fprintf(myFile,"Function 1: Convert all png files on the USB drive\r\n");
fprintf(myFile,"Function 2: A4 Font format converter : Swop nibbles in the byte-wise data [7:4] and [3:0]\r\n");
fprintf(myFile,"Function 3: C-array converter : ie Reduces ARGB8888 (32 bit) to ARGB15555 (16 bit) - incl Alpha\r\n");
fprintf(myFile,"Function 4: read PNG files and convert to C-array by filename\r\n\r\n");
fprintf(myFile,"https://github.com/javos65/H7-CArray-Converter-for-USBDisk\r\n");
fclose(myFile);
}

void printDirectory(char* name) {
  DIR *d;
  struct dirent *p;

  d = opendir(name);
  if (d != NULL) {
    while ((p = readdir(d)) != NULL) {
      Serial.println(p->d_name);
    }
  }
  closedir(d);
}

void loop() {
  if (MassStorage.media_removed()) {
    // list the content of the partitions
    // you may need to restart the board for the list to update if you copied new files
    Serial.println("Content of WiFi partition:");
    printDirectory("/wifi");
    Serial.println("Content of OTA partition:");
    printDirectory("/ota");
  }
  delay(1000);

}
