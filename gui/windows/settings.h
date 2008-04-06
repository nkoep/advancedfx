#ifndef HLAE_SETTINGS_H
#define HLAE_SETTINGS_H

#include <wx/wx.h>
#include <wx/statline.h>
#include <wx/treectrl.h>


class hlaeSettingsPageTemplate : public wxWindow 
{
public:
	hlaeSettingsPageTemplate(wxWindow* parent);
	virtual void ShowPage(bool is_advanced);
	virtual void ApplyChanges();
};

class hlaeSettingsPageGeneral : public hlaeSettingsPageTemplate 
{
	public:
		hlaeSettingsPageGeneral(wxWindow* parent);
		void ShowPage(bool is_advanced);
		void ApplyChanges();
};

class hlaeListElementSettingsPage : public wxObject
{
	private:
		wxList* m_pagelist;
		bool m_isadvanced;
		wxString m_name;
		hlaeSettingsPageTemplate* m_page;

	public:
		hlaeListElementSettingsPage(hlaeSettingsPageTemplate* window,
			const wxString& name, bool is_advanced);
		~hlaeListElementSettingsPage();
		bool IsAdvanced();
		const wxString& GetName();
		hlaeSettingsPageTemplate* GetPage();
		void Append(hlaeListElementSettingsPage* page_element);
		hlaeListElementSettingsPage* GetElement(int index);
		size_t GetCount();
};

class hlaeListElementSettingsPageID : public wxObject
{
public:
	wxTreeItemId id;
	hlaeSettingsPageTemplate* page;
};

class hlaeDialogSettings : public wxDialog
{
	private:

		DECLARE_EVENT_TABLE()

		enum {
			hlaeID_AdvancedMode = wxID_HIGHEST+1,
			hlaeID_SelectionChanged
		};

		void OnAdvancedMode(wxCommandEvent& evt);
		void OnApply(wxCommandEvent& evt);
		void OnOK(wxCommandEvent& evt);
		void OnSelectionChanged(wxTreeEvent& evt);

		void CreateForm();
		void UpdateTreeCtrl();
		void UpdateTreeCtrlNodes(hlaeListElementSettingsPage* node, wxTreeItemId id);

		wxList* m_pagelist;
		wxList* m_pageidlist;
		hlaeSettingsPageTemplate* m_lastpage;
		bool m_advancedmode;

		wxTreeCtrl* m_treectrl;
		wxFlexGridSizer* m_pagesizer;
		wxCheckBox* m_chkAdvanced;
		wxButton* m_btnOK;
		wxButton* m_btnCancel;
		wxButton* m_btnApply;
	
	public:

		hlaeDialogSettings( wxWindow* parent, wxWindowID id = wxID_ANY,
			const wxString& title = _T("Settings"),
			const wxPoint& pos = wxDefaultPosition,
			const wxSize& size = wxSize( 564,427 ));
		~hlaeDialogSettings();
		void Append(hlaeListElementSettingsPage* page_element);
	
};

#endif // _HLAE_DIALOGSETTINGS_H_