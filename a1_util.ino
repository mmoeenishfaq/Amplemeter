void openJsonString(String &j)
{
  j += "{";
}

void closeJsonString(String &j)
{
  j += "}";
}

void addJsonMember(const char * m, const char* value, String &j, bool last = false)
{
  j = j + "\"" + m + "\"" + ":";
  j = j + "\"" + value + "\"";
  if (!last) {
    j += ",";
  }
}

void addJsonArray(const char *m, double* item, int items, String &j, bool last = false)
{
  j = j + "\"" + m + "\"" + ":";
  j += "[";

  //  char buf[10];
  for (int i = 0; i < items - 1; i++) {
    //    sprintf(buf, "%.2f", item[i]);
    //    j += buf;
    j += item[i];
    j += ",";
  }

  // last item in the array
  //  sprintf(buf, "%.2f", item[items - 1]);
  //  j += buf;
  j += item[items - 1];

  j += "]";
  if (!last) {
    j += ",";
  }
}
