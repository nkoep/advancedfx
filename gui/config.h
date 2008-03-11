#ifndef _HLAE_CONFIG_H_
#define _HLAE_CONFIG_H_

#include "debug.h"
#include "list.h"

#include <wx/xml/xml.h>

class hlaeConfigListObject : public wxObject
{
public:
	hlaeConfigListObject(const wxString& name);
	const wxString& GetName();
private:
	wxString m_name;
};

class hlaeConfigListData : public hlaeConfigListObject
{
public:
	hlaeConfigListData(const wxString& name, const wxString& default_value);
	void SetData(const wxString& data);
	const wxString& GetData();
private:
	wxString m_data;
};

class hlaeConfigListGroup : public hlaeConfigListObject
{
public:
	hlaeConfigListGroup(const wxString& name);
	~hlaeConfigListGroup();
	void AppendObject(hlaeConfigListData* object);
	size_t GetCount();
	hlaeConfigListData* GetObject(size_t index);
private:
	hlaeList* m_datalist;
};

class hlaeConfig : public wxObject
{
public:
	hlaeConfig();
	~hlaeConfig();
	void AppendPropertyGroup(hlaeConfigListGroup* group);
	const wxString& GetString(const wxString& group_name, const wxString& property_name);
private:
	hlaeConfigListGroup* GetPropertyGroup(const wxString& group_name);
	const wxString& GetPropertyData(hlaeConfigListGroup* group, const wxString& property_name);
	hlaeList* m_propertylist;
	wxXmlDocument* m_document;
};

extern hlaeConfig g_config;

#endif