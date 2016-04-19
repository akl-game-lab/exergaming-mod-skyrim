scriptName MyPluginScript Hidden

int Function FetchXml(string gameID, string userName, string fromDate) global native

string Function CurrentDate() global native

bool Function IsFirstWeekCompleted(string firstImportDate, string workoutDate) global native

int Function dayOfTheWeek(string firstImportDate, string workoutDate) global native

string Function getXmlString(string fileName, string itemType, int index, string field) global native

int Function getItemCount(string fileName, string itemType) global native