#include <wx/wx.h>
#include <wx/treectrl.h>
#include <string>


class ContactNode 
{
    public:
        ContactNode(const std::string& firstName, const std::string& lastName, const std::string& phoneNumber, const std::string& address) : firstName_(firstName), lastName_(lastName), phoneNumber_(phoneNumber), address_(address) 
        {

        }

        std::string getFirstName() const
        {
            return firstName_;
        }

        std::string getLastName() const
        {
            return lastName_;
        }

        std::string getFullName() const 
        {
            return firstName_ + " " + lastName_;
        }

        std::string getPhoneNumber() const 
        {
            return phoneNumber_;
        }

        std::string getAddress() const 
        {
            return address_;
        }

        void setFirstName(const std::string& firstName)
        {
            firstName_ = firstName;
        }

        void setLastName(const std::string& lastName)
        {
            lastName_ = lastName;
        }

        void setPhoneNumber(const std::string& phoneNumber)
        {
            phoneNumber_ = phoneNumber;
        }

        void setAddress(const std::string& address)
        {
            address_ = address;
        }
        
        
    private:
        std::string firstName_;
        std::string lastName_;
        std::string phoneNumber_;
        std::string address_;
};

class ContactNodeData : public wxTreeItemData 
{
    public:
        ContactNodeData(ContactNode* contactNode) : contactNode_(contactNode) 
        {

        }

        ContactNode* GetContactNode() const 
        {
            return contactNode_;
        }


    private:
        ContactNode* contactNode_;
};

class TeleAddressWindow : public wxFrame
{
    public:
        TeleAddressWindow(const wxString& title, const wxPoint& pos, const wxSize& size) : wxFrame(nullptr, wxID_ANY, title, pos, size)
    {
        // Create contact tree
        contactTree_ = new wxTreeCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE | wxTR_FULL_ROW_HIGHLIGHT);

        //root of the tree
        wxTreeItemId rootItemId = contactTree_->AddRoot("Contacts");

        // Set up events
        contactTree_->Bind(wxEVT_TREE_SEL_CHANGED, &TeleAddressWindow::OnContactSelected, this);

        // Create controls to add new contact
        textCtrlFirstName_ = new wxTextCtrl(this, wxID_ANY, wxEmptyString);
        textCtrlLastName_ = new wxTextCtrl(this, wxID_ANY, wxEmptyString);
        textCtrlPhoneNumber_ = new wxTextCtrl(this, wxID_ANY, wxEmptyString);
        textCtrlAddress_ = new wxTextCtrl(this, wxID_ANY, wxEmptyString);
        buttonAdd_ = new wxButton(this, wxID_ANY, "Add");
        buttonAdd_->Bind(wxEVT_BUTTON, &TeleAddressWindow::OnAddButtonClicked, this);

        // Set the window layout
        wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
        sizer->Add(contactTree_, 1, wxEXPAND | wxALL, 5);

        wxStaticBoxSizer* addSizer = new wxStaticBoxSizer(wxVERTICAL, this, "Add Contact");
        addSizer->Add(new wxStaticText(this, wxID_ANY, "Name:"), 0, wxALL, 5);
        addSizer->Add(textCtrlFirstName_, 0, wxEXPAND | wxALL, 5);
        addSizer->Add(new wxStaticText(this, wxID_ANY, "Last Name:"), 0, wxALL, 5);
        addSizer->Add(textCtrlLastName_, 0, wxEXPAND | wxALL, 5);
        addSizer->Add(new wxStaticText(this, wxID_ANY, "Phone:"), 0, wxALL, 5);
        addSizer->Add(textCtrlPhoneNumber_, 0, wxEXPAND | wxALL, 5);
        addSizer->Add(new wxStaticText(this, wxID_ANY, "Address:"), 0, wxALL, 5);
        addSizer->Add(textCtrlAddress_, 0, wxEXPAND | wxALL, 5);
        addSizer->Add(buttonAdd_, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);

        sizer->Add(addSizer, 0, wxEXPAND | wxALL, 10);

        SetSizer(sizer);
        Centre();

        buttonEdit_ = new wxButton(this, wxID_ANY, "Edit");
        buttonEdit_->Connect(wxEVT_BUTTON, wxCommandEventHandler(TeleAddressWindow::OnEditButtonClicked), nullptr, this);
        addSizer->Add(buttonEdit_, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);

        buttonDelete_ = new wxButton(this, wxID_ANY, "Delete");
        buttonDelete_->Connect(wxEVT_BUTTON, wxCommandEventHandler(TeleAddressWindow::OnDeleteButtonClicked), nullptr, this);
        addSizer->Add(buttonDelete_, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);

    }

    void OnContactSelected(wxTreeEvent& event)
    {
        wxTreeItemId itemId = event.GetItem();
        ContactNodeData* contactData = dynamic_cast<ContactNodeData*>(contactTree_->GetItemData(itemId));
        if (contactData)
        {
            ContactNode* contact = contactData->GetContactNode();
            wxString fullName(contact->getFullName());
            wxString phoneNumber(contact->getPhoneNumber());
            wxString address(contact->getAddress());
            wxLogMessage("Selected contact: %s\nPhone: %s\nAddress: %s", fullName, phoneNumber, address);
        }
    }

    void OnAddButtonClicked(wxCommandEvent& event)
    {
        // Get the contact data from the input fields
        wxString firstName = textCtrlFirstName_->GetValue().ToStdString();
        wxString lastName = textCtrlLastName_->GetValue().ToStdString();
        wxString phoneNumber = textCtrlPhoneNumber_->GetValue().ToStdString();
        wxString address = textCtrlAddress_->GetValue().ToStdString();

        if(!firstName.empty() && !lastName.empty() && !phoneNumber.empty() && !address.empty())
        {
            if(editMode_)
            {
                // Get the contact selected in the tree
                wxTreeItemId itemId = contactTree_->GetSelection();
                if(itemId.IsOk())
                {
                    ContactNodeData* contactData = dynamic_cast<ContactNodeData*>(contactTree_->GetItemData(itemId));
                    if(contactData)
                    {
                        ContactNode* contact = contactData->GetContactNode();

                        //  Update the existing contact's data
                        contact->setFirstName(firstName.ToStdString());
                        contact->setLastName(lastName.ToStdString());
                        contact->setPhoneNumber(phoneNumber.ToStdString());
                        contact->setAddress(address.ToStdString());

                        // Update the contact data in the tree
                        contactTree_->SetItemText(itemId, contact->getFullName());
                    }
                }
            }
            else
            {
                // Create the new contact node
                ContactNode* contact = new ContactNode(firstName.ToStdString(), lastName.ToStdString(), phoneNumber.ToStdString(), address.ToStdString());

                // Insert the contact as a child of the tree root
                wxTreeItemId rootItemId = contactTree_->GetRootItem();
                wxTreeItemId newItemId = contactTree_->AppendItem(rootItemId, contact->getFullName(), -1, -1, new ContactNodeData(contact));
                contactTree_->SortChildren(rootItemId);
                // Select the new contact
                contactTree_->SelectItem(newItemId);
            }

            // Clear input fields
            textCtrlFirstName_->Clear();
            textCtrlLastName_->Clear();
            textCtrlPhoneNumber_->Clear();
            textCtrlAddress_->Clear();
    
            editMode_ = false;

        }
        else
        {
            wxMessageBox("Please fill in all the fields.", "Error", wxOK | wxICON_ERROR);
        } 
    }

    void OnEditButtonClicked(wxCommandEvent& event)
    {
        // Get the contact selected in the tree
        wxTreeItemId itemId = contactTree_->GetSelection();
        if (itemId.IsOk())
        {
            ContactNodeData* contactData = dynamic_cast<ContactNodeData*>(contactTree_->GetItemData(itemId));
            if (contactData)
            {
                ContactNode* contact = contactData->GetContactNode();

                // Load contact data in input fields
                textCtrlFirstName_->SetValue(contact->getFirstName());
                textCtrlLastName_->SetValue(contact->getLastName());
                textCtrlPhoneNumber_->SetValue(contact->getPhoneNumber());
                textCtrlAddress_->SetValue(contact->getAddress());

                // Set the Edit Mode
                editMode_ = true;
            }
        }
    }

    void OnDeleteButtonClicked(wxCommandEvent& event)
    {
        // Get the contact selected in the tree
        wxTreeItemId itemId = contactTree_->GetSelection();
        if (itemId.IsOk())
        {
            // Confirm the contact's deletion
            int answer = wxMessageBox("Are you sure you want to delete this contact?", "Confirm Deletion", wxYES_NO | wxICON_QUESTION);
            if (answer == wxYES)
            {
                // Delete the contact
                contactTree_->Delete(itemId);
            }
        }
    }

    private:
        wxTreeCtrl* contactTree_;
        wxTextCtrl* textCtrlFirstName_;
        wxTextCtrl* textCtrlLastName_;
        wxTextCtrl* textCtrlPhoneNumber_;
        wxTextCtrl* textCtrlAddress_;
        wxButton* buttonAdd_;
        wxButton* buttonEdit_;
        wxButton* buttonDelete_;
        bool editMode_;
};

class TeleAddressApp : public wxApp
{
    public:
        bool OnInit() override
        {
            TeleAddressWindow* mainWindow = new TeleAddressWindow("TeleAddress", wxPoint(50, 50), wxSize(800, 600));
            mainWindow->Show(true);
            return true;
        }
};

wxIMPLEMENT_APP(TeleAddressApp);