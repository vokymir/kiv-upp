#include "databaze.h"

Database::Database() {
  mValues.resize(Velikost_Databaze);
  for (int i = 0; i < mValues.size(); i++) {
    mValues[i] = i;
  }
}

long long Database::read(int index) const { return mValues[index]; }

void Database::write(int index, long long value) { mValues[index] = value; }
