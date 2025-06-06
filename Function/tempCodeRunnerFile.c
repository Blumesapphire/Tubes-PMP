  token = strtok(shiftStr, " ");
  int j = 0;
  char shiftJoin[100] = "";

  while (token && j < 3) {
    strcpy(newDokter.shift[j++], token);

    strcat(shiftJoin, token);
    token = strtok(NULL, " ");
    if (token != NULL) strcat(shiftJoin, ";");
  }
  while (j < 3) strcpy(newDokter.shift[j++], "");
