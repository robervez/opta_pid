
inline int StringSplit(String &strIn, String *strsOut, char separator){
  
  int StringCount = 0;
  strIn.replace("\r","");
  strIn.replace("\n","");
  // Split the string into substrings
  while (strIn.length() > 0)
  {
    int index = strIn.indexOf(separator);
    if (index == -1) // No space found
    {
      strsOut[StringCount++] = strIn;
      break;
    }
    else
    {
      strsOut[StringCount++] = strIn.substring(0, index);
      strIn = strIn.substring(index+1);
    }
  }
  return StringCount;
}