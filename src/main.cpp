#include <wx/wx.h>
#include <wx/app.h>
#include <wx/window.h>
#include <wx/treectrl.h>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>

class ContactNode 
{
    public:
        ContactNode(const std::string& firstName, const std::string& lastName, const std::string& phoneNumber, const std::string& address, const std::string& companyName, const std::string& companyPhone, const std::string& companyRif, const std::string& newEvent) : firstName_(firstName), lastName_(lastName), phoneNumber_(phoneNumber), address_(address), companyName_(companyName), companyPhone_(companyPhone), companyRif_(companyRif), newEvent_(newEvent)
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

        std::string getCompanyName() const
        {
            return companyName_;
        }

        std::string getCompanyPhone() const
        {
            return companyPhone_;
        }

        std::string getCompanyRif() const
        {
            return companyRif_;
        }

        std::string getNewEvent() const
        {
            return newEvent_;
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

        void setCompanyName(const std::string& companyName)
        {
            companyName_ = companyName;
        }

        void setCompanyPhone(const std::string& companyPhone)
        {
            companyPhone_ = companyPhone;
        }

        void setCompanyRif(const std::string& companyRif)
        {
            companyRif_ = companyRif;
        }

        void setNewEvent(const std::string& newEvent)
        {
            newEvent_ = newEvent;
        }
        
        bool operator<(const ContactNode& other) const
        {
            return getFullName() < other.getFullName();
        }
        
    private:
        std::string firstName_;
        std::string lastName_;
        std::string phoneNumber_;
        std::string address_;
        std::string companyName_;
        std::string companyPhone_;
        std::string companyRif_;
        std::string newEvent_;
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

class SearchWindow : public wxFrame
{
    public:
        SearchWindow(const wxString& title, const wxPoint& pos, const wxSize& size) : wxFrame(nullptr, wxID_ANY, title, pos, size)
        {
            // Create controls needed for search
            textCtrlSearch_ = new wxTextCtrl(this, wxID_ANY, wxEmptyString);
            buttonClose_ = new wxButton(this, wxID_ANY, "Close");
            listBoxResults_ = new wxListBox(this, wxID_ANY);

            // configure window layout
            wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
            sizer->Add(new wxStaticText(this, wxID_ANY, "Search Contacts:"), 0, wxALL, 5);
            sizer->Add(textCtrlSearch_, 0, wxEXPAND | wxALL, 5);
            sizer->Add(listBoxResults_, 1, wxEXPAND | wxALL, 5);
            sizer->Add(buttonClose_, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
            SetSizerAndFit(sizer);

            //Automatic frame update
            textCtrlSearch_->Connect(wxEVT_TEXT, wxCommandEventHandler(SearchWindow::OnSearchTextChanged), nullptr, this);

            // Connect search button event
            buttonClose_->Connect(wxEVT_BUTTON, wxCommandEventHandler(SearchWindow::OnCloseButtonClicked), nullptr, this);

            // Set window size and position
            int screenWidth, screenHeight;
            wxDisplaySize(&screenWidth, &screenHeight);
            int windowWidth = screenWidth / 2;
            int windowHeight = screenHeight / 2;
            int windowX = (screenWidth - windowWidth) / 2;
            int windowY = (screenHeight - windowHeight) / 2;
            SetSize(windowX, windowY, windowWidth, windowHeight);
        }

    private:
        
        wxTextCtrl* textCtrlSearch_;
        wxButton* buttonClose_;
        wxListBox* listBoxResults_;

        void OnSearchTextChanged(wxCommandEvent& event)
        {
            wxString searchText = textCtrlSearch_->GetValue();
            listBoxResults_->Clear();

            //Get matching contacts
            std::vector<wxString> matchingContacts;

            //Find the file and display the results in the tree.
            std::ifstream file("contacts.txt");
            if (file.is_open())
            {
                std::string line;
                while (std::getline(file, line))
                {
                    // Find the search string in each line of the file
                    if (line.find(searchText.ToStdString()) != std::string::npos)
                    {
                        //Add the matching contact to the tree
                        listBoxResults_->Append(wxString(line));
                    }
                }
                file.close();
            }

            //Sort the results
            std::sort(matchingContacts.begin(), matchingContacts.end());

            // Add the sorted results to the listBoxResults_
            for (const auto& contact : matchingContacts)
            {
                listBoxResults_->Append(contact);
            }

        }

        void OnCloseButtonClicked(wxCommandEvent& event)
        {
            Close();
        }
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
        companyNameCtrl_ = new wxTextCtrl(this, wxID_ANY, wxEmptyString);
        companyPhoneCtrl_ = new wxTextCtrl(this, wxID_ANY, wxEmptyString);
        companyRifCtrl_ = new wxTextCtrl(this, wxID_ANY, wxEmptyString);
        newEventCtrl_ = new wxTextCtrl(this,wxID_ANY, wxEmptyString);
        buttonAdd_ = new wxButton(this, wxID_ANY, "Add");
        buttonAdd_->Bind(wxEVT_BUTTON, &TeleAddressWindow::OnAddButtonClicked, this);
        
        // Set the window layout
        wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL); //HORIZONTAL
        sizer->Add(contactTree_, 1, wxEXPAND | wxALL, 5);

        //CheckBox belongs to a company
        companyCheckBox_ = new wxCheckBox(this, wxID_ANY, "Belongs to a company?");
        companySizer_ = new wxStaticBoxSizer(wxHORIZONTAL, this, "Company information");

        //CheckBox event
        eventCheckBox_ = new wxCheckBox(this, wxID_ANY, "Event?");
        eventSizer_ = new wxStaticBoxSizer(wxHORIZONTAL, this, "Event information");

        wxStaticBoxSizer* addSizer = new wxStaticBoxSizer(wxVERTICAL, this, "Add Contact");
        addSizer->GetStaticBox()->SetMinSize(wxSize(400, -1));
        SetForegroundColour(wxColour(255, 255, 255));
        SetBackgroundColour(wxColour(32, 31, 32));
        addSizer->Add(new wxStaticText(this, wxID_ANY, "Name:"), 0, wxALL, 5);
        addSizer->Add(textCtrlFirstName_, 0, wxEXPAND | wxALL, 5);
        addSizer->Add(new wxStaticText(this, wxID_ANY, "Last Name:"), 0, wxALL, 5);
        addSizer->Add(textCtrlLastName_, 0, wxEXPAND | wxALL, 5);
        addSizer->Add(new wxStaticText(this, wxID_ANY, "Phone:"), 0, wxALL, 5);
        addSizer->Add(textCtrlPhoneNumber_, 0, wxEXPAND | wxALL, 5);
        addSizer->Add(new wxStaticText(this, wxID_ANY, "Address:"), 0, wxALL, 5);
        addSizer->Add(textCtrlAddress_, 0, wxEXPAND | wxALL, 5);
        addSizer->Add(companyCheckBox_, 0, wxALIGN_LEFT | wxALL, 5);
        addSizer->Add(eventCheckBox_, 0, wxALIGN_LEFT | wxALL, 5);
        addSizer->Add(companySizer_, 0, wxEXPAND | wxALL, 10);
        addSizer->Add(eventSizer_, 0, wxEXPAND | wxALL, 10);
        addSizer->Add(buttonAdd_, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
        sizer->Add(addSizer, 0, wxEXPAND | wxALL, 10);
        SetSizer(sizer);
        Centre();
        //Company box
        companySizer_->Add(new wxStaticText(this, wxID_ANY, "Company name:"), 0, wxALL, 5);
        companySizer_->Add(companyNameCtrl_, 0, wxALL, 5);
        companySizer_->Add(new wxStaticText(this, wxID_ANY, "Company phone:"), 0, wxALL, 5);
        companySizer_->Add(companyPhoneCtrl_, 0, wxALL, 5);
        companySizer_->Add(new wxStaticText(this, wxID_ANY, "Company rif:"), 0, wxALL, 5);
        companySizer_->Add(companyRifCtrl_, 0, wxALL,5);
        companySizer_->Show(false);
        //Event box
        eventSizer_->Add(new wxStaticText(this, wxID_ANY, "Event:"), 0, wxALL, 5);
        eventSizer_->Add(newEventCtrl_, 0, wxALL, 5);
        eventSizer_->Show(false);



        buttonOpenSearch_ = new wxButton(this, wxID_ANY, "Open Search");
        buttonOpenSearch_->Connect(wxEVT_BUTTON, wxCommandEventHandler(TeleAddressWindow::OnSearchButtonClicked), nullptr, this);
        addSizer->Add(buttonOpenSearch_, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);

        buttonEdit_ = new wxButton(this, wxID_ANY, "Edit");
        buttonEdit_->Connect(wxEVT_BUTTON, wxCommandEventHandler(TeleAddressWindow::OnEditButtonClicked), nullptr, this);
        addSizer->Add(buttonEdit_, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);

        buttonDelete_ = new wxButton(this, wxID_ANY, "Delete");
        buttonDelete_->Connect(wxEVT_BUTTON, wxCommandEventHandler(TeleAddressWindow::OnDeleteButtonClicked), nullptr, this);
        addSizer->Add(buttonDelete_, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);

        companyCheckBox_->Connect(wxEVT_CHECKBOX, wxCommandEventHandler(TeleAddressWindow::OnCompanyCheckBox), nullptr, this);

        eventCheckBox_->Connect(wxEVT_CHECKBOX, wxCommandEventHandler(TeleAddressWindow::OnEventCheckBox), nullptr, this);
        Maximize();

    }

    void OnCompanyCheckBox(wxCommandEvent& event)
    {
        bool isChecked = companyCheckBox_->GetValue();

        // Show or hide the companySizer_
        companySizer_->Show(isChecked);

        // Rearrange the window layout
        Layout();
    }

    void OnEventCheckBox(wxCommandEvent& event)
    {
        bool isChecked = eventCheckBox_->GetValue();

        // Show or hide the eventSizer_
        eventSizer_->Show(isChecked);

        // Rearrange the window layout
        Layout();
    }

    void LoadContactsFromFile()
    {
        std::ifstream inputFile(fileName);
        if (inputFile.is_open())
        {
            std::vector<std::pair<std::string, std::string>> contacts;
            std::string line;
            while (std::getline(inputFile, line))
            {
                // Separate the line in contact fields
                std::istringstream iss(line);
                std::string fullName, phoneNumber, address, companyName, companyPhone, companyRif, newEvent;
                std::getline(iss, fullName, ',');
                std::getline(iss, phoneNumber, ',');
                std::getline(iss, address, ',');
                std::getline(iss, companyName, ',');
                std::getline(iss, companyPhone, ',');
                std::getline(iss, companyRif, ',');
                std::getline(iss, newEvent, ',');

                // Add the contact to the vector
                contacts.push_back(std::make_pair(fullName, line));
            }
            inputFile.close();

            // Sort contacts alphabetically
            std::sort(contacts.begin(), contacts.end(), [](const std::pair<std::string, std::string>& a, const std::pair<std::string, std::string>& b) 
            {
                    return a.first < b.first;
            });

            // Add contacts to the tree in alphabetical order
            wxTreeItemId rootItemId = contactTree_->GetRootItem();
            for (const auto& contact : contacts)
            {
                wxString fullName(contact.first);
                wxString contactInfo(contact.second);

                // Create a new ContactNode object using the contact data
                std::istringstream iss(contact.second);
                std::string firstName, lastName, phoneNumber, address, companyName, companyPhone, companyRif, newEvent;
                std::getline(iss, firstName, ',');
                std::getline(iss, lastName, ',');
                std::getline(iss, phoneNumber, ',');
                std::getline(iss, address, ',');
                if(iss)
                {
                    std::getline(iss, companyName, ',');
                    std::getline(iss, companyPhone, ',');
                    std::getline(iss, companyRif, ',');
                    std::getline(iss, newEvent, ',');
                }
                else
                {
                    companyName = "";
                    companyPhone = "";
                    companyRif = "";
                    newEvent = "";
                }
                
                ContactNode* contactNode = new ContactNode(firstName, lastName, phoneNumber, address, companyName, companyPhone, companyRif, newEvent);

                // Create ContactNodeData with ContactNode object
                contactTree_->AppendItem(rootItemId, fullName, -1, -1, new ContactNodeData(contactNode));
            }
        }
    }

    wxTreeCtrl* GetContactTree() const 
    { 
        return contactTree_; 
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
            wxString companyName(contact->getCompanyName());
            wxString companyPhone(contact->getCompanyPhone());
            wxString companyRif(contact->getCompanyRif());
            wxString newEvent(contact->getNewEvent());
            if(contact->getCompanyName() == "" || contact->getCompanyPhone() == "" || contact->getCompanyRif() == "" || contact->getNewEvent() == "")
            {
                wxLogMessage("Selected contact: %s\nPhone: %s\nAddress: %s", fullName, phoneNumber, address);
            }
            else
            {
                wxLogMessage("Selected contact: %s\nPhone: %s\nAddress: %s\nCompany Name: %s\nCompany Phone: %s\nCompany Rif: %S\nEvent: %s", fullName, phoneNumber, address, companyName, companyPhone, companyRif, newEvent);
                
            }
            
        }
    }

    void SaveContactsToFile()
    {
        //loops through the contacts stored in the tree and writes their data to a file
        std::ofstream outputFile(fileName);
        if(outputFile.is_open())
        {
            wxTreeItemIdValue cookie;
            wxTreeItemId rootItemId = contactTree_->GetRootItem();
            wxTreeItemId itemId = contactTree_->GetFirstChild(rootItemId, cookie);
            while(itemId.IsOk())
            {
                ContactNodeData* contactData = dynamic_cast<ContactNodeData*>(contactTree_->GetItemData(itemId));
                if(contactData)
                {
                    ContactNode* contact = contactData->GetContactNode();
                    outputFile << contact->getFirstName() << "," << contact->getLastName() << "," << contact->getPhoneNumber() << "," << contact->getAddress() << "," << contact->getCompanyName() << "," << contact->getCompanyPhone() << "," << contact->getCompanyRif() << "," << contact->getNewEvent() << "\n";
                }
                itemId = contactTree_->GetNextChild(rootItemId, cookie);
            }
            outputFile.close();
        }
    }

    void OnAddButtonClicked(wxCommandEvent& event)
    {
        // Get the contact data from the input fields
        wxString firstName = textCtrlFirstName_->GetValue().ToStdString();
        wxString lastName = textCtrlLastName_->GetValue().ToStdString();
        wxString phoneNumber = textCtrlPhoneNumber_->GetValue().ToStdString();
        wxString address = textCtrlAddress_->GetValue().ToStdString();
        wxString companyName = companyNameCtrl_->GetValue();
        wxString companyPhone = companyPhoneCtrl_->GetValue();
        wxString companyRif = companyRifCtrl_->GetValue();
        wxString newEvent = newEventCtrl_->GetValue();
        bool belongsToCompany = companyCheckBox_->GetValue();
        bool hasEvent = eventCheckBox_->GetValue();
        ContactNode* contact;

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
                        contact = contactData->GetContactNode();

                        //  Update the existing contact's data
                        contact->setFirstName(firstName.ToStdString());
                        contact->setLastName(lastName.ToStdString());
                        contact->setPhoneNumber(phoneNumber.ToStdString());
                        contact->setAddress(address.ToStdString());
                        if(belongsToCompany)
                        {
                            if(!companyName.empty() && !companyPhone.empty() && !companyRif.empty())
                            {
                                contact->setCompanyName(companyName.ToStdString());
                                contact->setCompanyPhone(companyPhone.ToStdString());
                                contact->setCompanyRif(companyRif.ToStdString());
                            }
                            else
                            {
                                wxMessageBox("Please fill in all the fields.", "Error", wxOK | wxICON_ERROR);
                            }
                            
                        }
                        if(hasEvent)
                        {
                            if(!newEvent.empty())
                            {
                                contact->setNewEvent(newEvent.ToStdString());
                            }
                            else
                            {
                                wxMessageBox("Please fill in all the fields.", "Error", wxOK | wxICON_ERROR);
                            }
                        }

                        // Update the contact data in the tree
                        contactTree_->SetItemText(itemId, contact->getFullName());
                    }
                }
                buttonAdd_->SetLabel("Add");
            }
            else
            {
                // Create the new contact node
                if(!belongsToCompany && !hasEvent)
                {
                    contact = new ContactNode(firstName.ToStdString(), lastName.ToStdString(), phoneNumber.ToStdString(), address.ToStdString(), "", "", "", "");
                }
                else if (belongsToCompany && !hasEvent)
                {
                    contact = new ContactNode(firstName.ToStdString(), lastName.ToStdString(), phoneNumber.ToStdString(), address.ToStdString(), companyName.ToStdString(), companyPhone.ToStdString(), companyRif.ToStdString(), "");
                }
                else
                {
                    contact = new ContactNode(firstName.ToStdString(), lastName.ToStdString(), phoneNumber.ToStdString(), address.ToStdString(), companyName.ToStdString(), companyPhone.ToStdString(), companyRif.ToStdString(), newEvent.ToStdString());
                }
                

                // Insert the contact as a child of the tree root
                wxTreeItemId rootItemId = contactTree_->GetRootItem();
                wxTreeItemId newItemId = contactTree_->AppendItem(rootItemId, contact->getFullName(), -1, -1, new ContactNodeData(contact));
                //Sort the children of the root alphabetically
                contactTree_->SortChildren(rootItemId);
                // Select the new contact
                contactTree_->SelectItem(newItemId);
            }

            // Clear input fields
            textCtrlFirstName_->Clear();
            textCtrlLastName_->Clear();
            textCtrlPhoneNumber_->Clear();
            textCtrlAddress_->Clear();
            companyNameCtrl_->Clear();
            companyPhoneCtrl_->Clear();
            companyRifCtrl_->Clear();
            newEventCtrl_->Clear();

            editMode_ = false;

            SaveContactsToFile();
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
        ContactNode* contact;

        if (itemId.IsOk())
        {
            ContactNodeData* contactData = dynamic_cast<ContactNodeData*>(contactTree_->GetItemData(itemId));
            if (contactData)
            {
                contact = contactData->GetContactNode();
                

                // Load contact data in input fields
                textCtrlFirstName_->SetValue(contact->getFirstName());
                textCtrlLastName_->SetValue(contact->getLastName());
                textCtrlPhoneNumber_->SetValue(contact->getPhoneNumber());
                textCtrlAddress_->SetValue(contact->getAddress());
                companyNameCtrl_->SetValue(contact->getCompanyName());
                companyPhoneCtrl_->SetValue(contact->getCompanyPhone());
                companyRifCtrl_->SetValue(contact->getCompanyRif());
                newEventCtrl_->SetValue(contact->getNewEvent());
                if(!companyNameCtrl_->IsEmpty())
                {
                    companyCheckBox_->SetValue(true);
                    // Show or hide the companySizer_
                    companySizer_->Show(true);

                    // Rearrange the window layout
                    Layout();
                }
                else
                {
                    companyCheckBox_->SetValue(false);
                    companySizer_->Show(false);
                }
                if(!newEventCtrl_->IsEmpty())
                {
                    eventCheckBox_->SetValue(true);
                    // Show or hide the eventSizer_
                    eventSizer_->Show(true);

                    // Rearrange the window layout
                    Layout();
                }
                else
                {
                    eventCheckBox_->SetValue(false);
                    eventSizer_->Show(false);
                }
                

                // Set the Edit Mode and buttonAdd_ value update
                editMode_ = true;
                buttonAdd_->SetLabel("Update");
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

        //loops through the contacts stored in the tree and writes their data to a file
        std::ofstream outputFile(fileName);
        if(outputFile.is_open())
        {
            wxTreeItemIdValue cookie;
            wxTreeItemId rootItemId = contactTree_->GetRootItem();
            wxTreeItemId itemId = contactTree_->GetFirstChild(rootItemId, cookie);
            while (itemId.IsOk())
            {
                ContactNodeData* contactData = dynamic_cast<ContactNodeData*>(contactTree_->GetItemData(itemId));
                if(contactData)
                {
                    ContactNode* contact = contactData->GetContactNode();
                    outputFile << contact->getFirstName() << "," << contact->getLastName() << "," << contact->getPhoneNumber() << "," << contact->getAddress() << "," << contact->getCompanyName() << "," << contact->getCompanyPhone() << "," << contact->getCompanyRif() << "," << contact->getNewEvent() << "\n";
                }
                itemId = contactTree_->GetNextChild(rootItemId, cookie);
            }
            outputFile.close();
        }

    }


    void OnSearchButtonClicked(wxCommandEvent& event)
    {
        if(!searchWindow_)
        {
            //create search window
            searchWindow_ = new SearchWindow("Search Contacts", wxPoint(50, 50), wxSize(400, 200));
            searchWindow_->Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(TeleAddressWindow::OnSearchWindowClosed), nullptr, this);
            searchWindow_->Show();
        }
        else
        {
            //Place main search window
            searchWindow_->Show();
            searchWindow_->Iconize(false);
            searchWindow_->Raise();
        }
        
    }
    void OnSearchWindowClosed(wxCloseEvent& event)
    {
        //Delete the search window instance when closing it
        searchWindow_->Destroy();
        searchWindow_ = nullptr;
    }

    private:
        wxTreeCtrl* contactTree_;
        wxTextCtrl* textCtrlFirstName_;
        wxTextCtrl* textCtrlLastName_;
        wxTextCtrl* textCtrlPhoneNumber_;
        wxTextCtrl* textCtrlAddress_;
        wxCheckBox* companyCheckBox_;
        wxCheckBox* eventCheckBox_;
        wxBoxSizer* companySizer_;
        wxBoxSizer* eventSizer_;
        wxTextCtrl* companyNameCtrl_;
        wxTextCtrl* companyPhoneCtrl_;
        wxTextCtrl* companyRifCtrl_;
        wxTextCtrl* newEventCtrl_;
        wxButton* buttonAdd_;
        wxButton* buttonEdit_;
        wxButton* buttonDelete_;
        wxButton* buttonOpenSearch_;
        SearchWindow* searchWindow_;
        bool editMode_;
        std::string fileName = "contacts.txt";
};


class TeleAddressApp : public wxApp
{
    public:
        std::string fileName = "contacts.txt";

        //check if the file exists  
        bool fileExists(const std::string& fileName)
        {
            std::ifstream file(fileName);
            return file.good();
        }

        bool OnInit() override
        {
            

            //if it does not exist, create the file
            if(!fileExists(fileName))
            {
                std::ofstream file(fileName);
            }
            
            

            TeleAddressWindow* mainWindow = new TeleAddressWindow("TeleAddress", wxPoint(50, 50), wxSize(800, 600));
            mainWindow->Show(true);

            std::vector<ContactNodeData*> contactNodes;
            std::ifstream inputFile(fileName);
            std::string line;
            while(std::getline(inputFile, line))
            {
                std::istringstream iss(line);
                std::string firstName, lastName, phoneNumber, address, companyName, companyPhone, companyRif, newEvent;

                if(std::getline(iss, firstName, ',') && std::getline(iss, lastName, ',') && std::getline(iss, phoneNumber, ',') && std::getline(iss, address, ',') && std::getline(iss, companyName, ',') && std::getline(iss, companyPhone, ',') && std::getline(iss, companyRif, ',') && std::getline(iss, newEvent, ','))
                {
                    ContactNode* contact = new ContactNode(firstName, lastName, phoneNumber, address, companyName, companyPhone, companyRif, newEvent);
                    contactNodes.push_back(new ContactNodeData(contact));
                }
                else
                {
                    ContactNode* contact = new ContactNode(firstName, lastName, phoneNumber, address, companyName, companyPhone, companyRif, newEvent);
                    contactNodes.push_back(new ContactNodeData(contact));
                }
            }
            inputFile.close();

            // Sort contacts alphabetically
            std::sort(contactNodes.begin(), contactNodes.end(), [](const ContactNodeData* a, const ContactNodeData* b) 
            {
            return a->GetContactNode()->getFullName() < b->GetContactNode()->getFullName();
            });

            // Add the sorted contacts to the tree
            wxTreeItemId rootItemId = mainWindow->GetContactTree()->GetRootItem();
            for (const auto& contactData : contactNodes)
            {
                ContactNode* contact = contactData->GetContactNode();
                wxString fullName(contact->getFullName());
                mainWindow->GetContactTree()->AppendItem(rootItemId, fullName, -1, -1, contactData);
            }

            return true;
        }
        
};

wxIMPLEMENT_APP(TeleAddressApp);