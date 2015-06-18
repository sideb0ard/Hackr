// dumps raw memory in hex bytes and printable split format
void dump(const unsigned char *data_buffer, const unsigned int length)
{
  unsigned char byte;
  unsigned int i, j;
  for(i=0; i < length; i++) {
    byte = data_buffer[i];
    printf("%02x ", data_buffer[i]); // display byte in hex
    if(((i%16)==15) || (i==length-1)) {
      for(j=0; j < 15-(i%16); j++)
        printf("  ");
      printf("| ");
      for(j=(i-(i%16)); j <= i; j++) { // display printable bytes from line
        byte = data_buffer[j];
        if((byte > 31) && (byte < 127)) // outside printable char range
          printf("%c", byte);
        else
          printf(".");
      }
      printf("\n"); // end of dump line, each line is 16 bytes
    } // end if
  } // end for
}

void fatal(const unsigned char *msg)
{
  printf("Died! reason: %s", msg);
  exit(1);
}
