/****************************************
* ФИ: Лучникова София                   *
* Название: Работа с файлами            *
* Вариант: 13                           *
****************************************/

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>

using namespace std;

struct Phone {
    string maker;
    string model;
    double weight;
    double price;
};

vector<string> splitFields(const string& line) {
  vector<string> fields;
  size_t start = 0;
  while (true) {
    size_t separator = line.find(';', start);
    if (separator == string::npos) {
      fields.push_back(line.substr(start));
      break;
    }
    fields.push_back(line.substr(start, separator - start));
    start = separator + 1;
  }
  return fields;
}

bool readDouble(const string& text, double& value) {
  istringstream parser(text);
  char extra;
  if (!(parser >> value) || !isfinite(value)) {
    return false;
  }
  return !(parser >> extra);
}

bool readInt(const string& text, int& value) {
  istringstream parser(text);
  char extra;
  if (!(parser >> value)) {
    return false;
  }
  return !(parser >> extra);
}

bool readPhones(const string& path, vector<Phone>& records) {
  ifstream input(path);
  if (!input) {
    cout << "ERROR open\n";
    return false;
  }

  string line;
  int lineNumber = 0;
  while (getline(input, line)) {
    ++lineNumber;
    if (!line.empty() && line.back() == '\r') {
      line.pop_back();
    }
    if (line.empty()) {
      cout << "ERROR line " << lineNumber << "\n";
      return false;
    }

    vector<string> fields = splitFields(line);
    if (fields.size() != 4) {
      cout << "ERROR line " << lineNumber << "\n";
      return false;
    }

    Phone current{};
    current.maker = fields[0];
    current.model = fields[1];

    if (current.maker.empty() || current.model.empty()) {
      cout << "ERROR line " << lineNumber << "\n";
      return false;
    }
    if (!readDouble(fields[2], current.weight) || current.weight <= 0 || current.weight > 100000) {
      cout << "ERROR line " << lineNumber << "\n";
      return false;
    }
    if (!readDouble(fields[3], current.price) || current.price <= 0 || current.price > 10000000) {
      cout << "ERROR line " << lineNumber << "\n";
      return false;
    }

    records.push_back(current);
  }

  if (input.bad()) {
    cout << "ERROR read\n";
    return false;
  }
  return true;
}

void buildReport(const vector<Phone>& records, const string& maker, ostringstream& report) {
  report << "Phones by maker: " << maker << "\n";

  double makerTotal = 0.0;
  double makerMinWeight = 0.0;
  bool makerMinFound = false;
  int makerCount = 0;

  for (size_t index = 0; index < records.size(); ++index) {
    if (records[index].maker == maker) {
      ++makerCount;
      report << records[index].maker << "; " << records[index].model << "; "
      << records[index].weight << " g; " << records[index].price << " RUB\n";
      makerTotal += records[index].price;
      if (!makerMinFound || records[index].weight < makerMinWeight) {
        makerMinWeight = records[index].weight;
        makerMinFound = true;
      }
    }
  }

  if (makerCount == 0) {
    report << "EMPTY\n";
    report << "Average price: NONE\n";
    report << "Lightest: NONE\n";
  }
  else {
    report << "Average price: " << makerTotal / makerCount << " RUB\n";
    for (size_t index = 0; index < records.size(); ++index) {
      if (records[index].maker == maker && records[index].weight == makerMinWeight) {
        report << "Lightest: " << records[index].model << " (" << makerMinWeight << " g)\n";
        break;
      }
    }
  }

  if (records.empty()) {
    report << "Cheapest overall: NONE\n";
    report << "Most expensive overall: NONE\n";
    return;
  }

  double minPrice = records[0].price;
  double maxPrice = records[0].price;

  for (size_t index = 1; index < records.size(); ++index) {

    if (records[index].price < minPrice) {
      minPrice = records[index].price;
    }
    if (records[index].price > maxPrice) {
      maxPrice = records[index].price;
    }
  }

  for (size_t index = 0; index < records.size(); ++index) {
    if (records[index].price == minPrice) {
      report << "Cheapest overall: " << records[index].maker << " " << records[index].model
             << " (" << minPrice << " RUB)\n";
      break;
    }
  }
  for (size_t index = 0; index < records.size(); ++index) {
    if (records[index].price == maxPrice) {
      report << "Most expensive overall: " << records[index].maker << " " << records[index].model
             << " (" << maxPrice << " RUB)\n";
      break;
    }
  }
}

int main() {
  const int kMaxRecords = 100;

  string inputPath;
  string outputPath;
  string maker;

  cout << "Input file path:\n";
  cin >> inputPath;

  cout << "Output file path:\n";
  cin >> outputPath;

  cout << "Maker query:\n";
  cin >> maker;

  vector<Phone> records;
  if (!readPhones(inputPath, records)) {
    return 1;
  }
  if (records.size() > static_cast<size_t>(kMaxRecords)) {
    cout << "ERROR limit\n";
    return 1;
  }

  ostringstream report;
  buildReport(records, maker, report);

  ofstream output(outputPath);
  if (!output) {
    cout << "ERROR output\n";
    return 1;
  }
  output << report.str();
  output.flush();
  if (!output) {
    cout << "ERROR write\n";
    return 1;
  }

  cout << report.str();
  return 0;
}