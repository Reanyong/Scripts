#pragma once
extern bool CheckValidTagName(const char* TagName);
extern bool KWPutTagValue(const char* TagName, c_variable TagValue);
extern bool KWGetTagValue(const char* TagName, c_variable& TagValue);
extern bool KWGetTagValue(const char* TagName, c_variable& TagValue);
extern bool KWSetTagField(const char* TagName, TAG_FIELD field, c_variable FieldValue);
extern bool KWGetTagField(const char* TagName, TAG_FIELD field, c_variable& FieldValue);
extern bool KWSendMsgToViewer(COPYDATASTRUCT cds);
