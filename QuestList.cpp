/*
CSCI235 Fall 2023
Project 5 - Quest Management with Doubly Linked Lists
Ashish Pal
Oct 29 2023
QuestList.cpp defines the constructors and private and public function implementation of the Character class
*/

#include "QuestList.hpp"
#include "Node.hpp"
#include "DoublyLinkedList.hpp"

/**
    Default Constructor
*/

QuestList::QuestList() : DoublyLinkedList<Quest*>()
{
}

/**
    @param: a reference to string name of an input file
    @pre: Formatting of the csv file is as follows:
        Title: A string
        Description: A string
        Completion Status: 0 (False) or 1 (True)
        Experience Points: A non negative integer
        Dependencies: A list of Quest titles of the form [QUEST1];[QUEST2], where each quest is separated by a semicolon. The value may be NONE.
        Subquests: A list of Quest titles of the form [QUEST1];[QUEST2], where each quest is separated by a semicolon. The value may be NONE.
    Notes:
        - The first line of the input file is a header and should be ignored.
        - The dependencies and subquests are separated by a semicolon and may be NONE.
        - The dependencies and subquests may be in any order.
        - If any of the dependencies or subquests are not in the list, they should be created as new quests with the following information:
            - Title: The title of the quest
            - Description: "NOT DISCOVERED"
            - Completion Status: False
            - Experience Points: 0
            - Dependencies: An empty vector
            - Subquests: An empty vector
        - However, if you eventually encounter a quest that matches one of the "NOT DISCOVERED" quests while parsing the file, you should update all the quest details.
        Hint: update as needed using addQuest()
        

    @post: Each line of the input file corresponds to a quest to be added to the list. No duplicates are allowed.

*/

QuestList::QuestList(const string& file)
{
    ifstream fin(file);

    if (!fin.is_open()) 
    {
        cerr << "Error: Unable to open input file." << endl;
        return;
    }
    string line;

    getline(fin, line);
    
    while(getline(fin, line))
    {
        stringstream ss(line);
        
        string filler, title, description, string_dependencies, string_subquests;
        bool complete;
        int experience_points;

        getline(ss, title, ',');
        getline(ss, description, ',');
        getline(ss, filler, ',');
        istringstream(filler) >> complete;
        getline(ss, filler, ',');
        istringstream(filler) >> experience_points;

        getline(ss, string_dependencies, ',');
        getline(ss, filler, ',');

        string_subquests = filler;
        string_subquests.erase(string_subquests.find_last_not_of(" \n\r\t") + 1);


        vector<Quest*> dependencies = helperVectors(string_dependencies);
        vector<Quest*> subquests = helperVectors(string_subquests);

        int position = getPosOf(title);
        Quest* quest;
        if (position == -1)
        {
            quest = new Quest(title, description, complete, experience_points, dependencies, subquests);
            insert(item_count_,quest);
        }
        else
        {
            quest = getPointerTo(position)->getItem();
            
            if (quest->description_ == "NOT DISCOVERED") 
            {
                quest->description_ = description;
                quest->completed_ = complete;
                quest->experience_points_ = experience_points;
            }
            quest->dependencies_ = dependencies;
            quest->subquests_ = subquests;
        }
    }

    fin.close();
}

/**
    @param: A string reference to a quest's dependency or subquest
    @return: A vector of dependencies or subquests that becomes updates whether  it is in the list or not
*/
vector<Quest*> QuestList::helperVectors(const string& dependencies)
{
    vector<Quest*> depend;
    istringstream fin(dependencies);
    string dependency;

    while(getline(fin, dependency, ';'))
    {
        if (dependency == "" || dependency == "NONE")
        {
            continue;
        }

        Quest* quest;
        if (getPosOf(dependency) == -1)
        {
            quest = new Quest(dependency, "NOT DISCOVERED", false, 0, {}, {});
            insert(item_count_, quest);
        }

        else
        {
            quest = getPointerTo(getPosOf(dependency))->getItem();

        }

        this->addQuest(quest);
        depend.push_back(quest);
    }
    return depend;
}

/**
    @param: A string reference to a quest title
    @return: The integer position of the given quest if it is in the QuestList, -1 if not found.
*/

int QuestList::getPosOf(const string& title) const
{
    Node<Quest*>* ptr = getHeadNode();
    int pos = 0;
    while (ptr)
    {
        if (title == ptr->getItem()->title_)
        {
            return pos;
        }

        ptr = ptr->getNext();
        pos++;
    }

    return -1;
}

/**
    @param: A string reference to a quest title
    @return: True if the quest with the given title is already in the QuestList
*/

bool QuestList::contains(const string& title) const
{
    if (getPosOf(title) == -1)
    {
        return false;
    }
    else
    {
        return true;
    }
}

/**
    @pre: The given quest is not already in the QuestList
    @param:  A pointer to a Quest object
    @post:  Inserts the given quest pointer into the QuestList. Each of its dependencies and subquests are also added to the QuestList IF not already in the list.
            If the quest is already in the list but is marked as "NOT DISCOVERED", update its details. (This happens when a quest has been added to the list through a dependency or subquest list)
           
    @return: True if the quest was added successfully, False otherwise
*/

bool QuestList::addQuest(Quest* const& quest)
{
    if (getPosOf(quest->title_) == -1)
    {
        insert(item_count_, quest);

        for(auto& dependencies : quest->dependencies_)
        {
            if(contains(dependencies->title_) == false)
            {
                addQuest(dependencies);
            }
        }

        for(auto& subquests : quest->subquests_)
        {
            if(contains(subquests->title_) == false)
            {
                addQuest(subquests);
            }
        }

        return true;
    }
    
    else
    {
        Node<Quest*>* ptr_node = getPointerTo(getPosOf(quest->title_));
        Quest* ptr_quest = ptr_node->getItem();

        if (ptr_quest->description_ == "NOT DISCOVERED")
        {
            ptr_quest->description_ = quest->description_;
            ptr_quest->completed_ = quest->completed_;
            ptr_quest->experience_points_ = quest->experience_points_;
            ptr_quest->dependencies_ = quest->dependencies_;
            ptr_quest->subquests_ = quest->subquests_;
        }
        
        return true;
    }

    return false;
}

/**
    @param:  A reference to string representing the quest title
    @param:  A reference to string representing the quest description
    @param:  A reference to boolean representing if the quest is completed
    @param:  An reference to int representing experience points the quest rewards upon completion 
    @param:  A reference to vector of Quest pointers representing the quest's dependencies
    @param:  A reference to vector of Quest pointers representing the quest's subquests
    @post:   Creates a new Quest object and inserts a pointer to it into the QuestList. 
             If the quest is already in the list but is marked as "NOT DISCOVERED", update its details. (This happens when a quest has been added to the list through a dependency or subquest list)
             Each of its dependencies and subquests are also added to the QuestList IF not already in the list.
             

    @return: True if the quest was added successfully

*/

bool QuestList::addQuest(const string& title, const string& description, const bool& complete, const int& experience_points, const vector<Quest*>& dependencies, const vector<Quest*>& subquests)
{
    Quest* quest = new Quest(title, description, complete, experience_points, dependencies, subquests);
    return addQuest(quest);
}

/**
    @param:  A Quest pointer
    @return: A boolean indicating if all the given quest's dependencies are completed
*/

bool QuestList::dependenciesComplete(Quest* const& quest) const
{
    for (Quest* dependencies : quest->dependencies_)
    {
        if ((dependencies->completed_ == false) || (contains(dependencies->title_) == false))
        {
            return false;
        }
    }
    return true;
}

/**
    @param: A Quest pointer
    @return: A boolean if the given quest is available.
    Note: For a quest to be available, it must not be completed, and its dependencies must be complete.
*/

bool QuestList::questAvailable(Quest* const& quest) const
{
    if (quest->completed_ == false)
    {
        if (dependenciesComplete(quest))
        {
            return true;
        }
    }
    return false;
}
/**
    @param: A Quest pointer
    @post: Prints the quest title and completion status
    The output should be of the form:
    [Quest Title]: [Complete / Not Complete]
    [Quest Description]\n\n
*/

void QuestList::printQuest(Quest* const& quest) const
{
    cout<<quest->title_<<": "<<(quest->completed_ ? "Complete" : "Not Complete")<<endl;
    cout<<quest->description_<<"\n\n";
}

/**
    @param: A string reference to a quest title
    @post:  Prints a list of quests that must to be completed before the given quest can be started (incomplete dependencies).
            If any of the quest's incomplete dependencies have an incomplete dependency, recursively print the quests that need to be done in order, indenting incomplete quests. 
            The indentation for incomplete quests is 2 spaces: "  "    
            The format of the list should be of the following forms for each different case:

            Query: [Quest Title]
            No such quest.
    
            Query: [Quest Title]
            Quest Complete
    
            Query: [Quest Title]
            Ready: [Quest Title]
    
            Query: [Quest Title]
            Ready: [Dependency0]
              [Quest Title]
            
            Query: [Quest Title]
            Ready: [Dependency0]
            Ready: [Dependency1]
              [Quest Title]
    
            Query: [Quest Title]
            Ready: [Dependency0]
            Ready: [Dependency1]
              [Dependency2]
              [Quest Title]
              
     If the given quest title is not found in the list, print "No such quest."
*/

void QuestList::questQuery(const string& title) const
{
    cout<<"Query: "<<title<<endl;

    if (getPosOf(title) == -1)
    {
        cout<<"No such quest.\n";
        return;
    }

    Quest* quest = getPointerTo(getPosOf(title))->getItem();

    if(quest->completed_)
    {
        cout<<"Quest Complete\n";
    }
    else
    {
        helperQuestQuery(quest);
    }
}


/**
    @param: A Quest pointer
    @return: Recurrsively prints out whether a dependency is ready and until there are no more dependencies or the dependencies are complete, it wont stop
*/

void QuestList::helperQuestQuery(Quest* const& quest) const {
    
    if ((quest->dependencies_.size() == 0) || (dependenciesComplete(quest)))
    {
        cout<<"Ready: "<<quest->title_<<endl;
        return;
    }

    for (int i=0;i<quest->dependencies_.size();i++)
    {
        if(quest->dependencies_[i]->completed_ == false)
        {
            helperQuestQuery(quest->dependencies_[i]);
        }
    }
    cout<<" "<<quest->title_<<endl;
}

/**
    @return: An integer sum of all the experience gained
    Note: This should only include experience from completed quests 
*/

int QuestList::calculateGainedExperience() const
{
    int exp = 0;
    Node<Quest*>* ptr = getHeadNode();
    while(ptr != nullptr)
    {
        if (ptr->getItem()->completed_ == true)
        {
            exp += ptr->getItem()->experience_points_;
        }
        ptr = ptr->getNext();
    }
    return exp;
}

/**
    @param: A quest pointer to a main quest
    @return: An integer sum of all the experience that can be gained from completing the main quest AND all its subquests.
    Note: Also consider the potential experience if a subquest itself has subquests.
*/

int QuestList::calculateProjectedExperience(Quest* const& quest) const
{
    int exp = 0;
    exp += quest->experience_points_;
    for (Quest* subquests : quest->subquests_)
    {
        exp += calculateProjectedExperience(subquests);
    }
    return exp;
}

/**
    @param: A quest pointer to a main quest
    @return: An integer sum of all the experience that has already been gained by completing the given quest's subquests.
    Note: Also consider the experience gained if a completed subquest itself has subquests.  
*/

int QuestList::calculatePathwayExperience(Quest* const& quest) const
{
    int exp = 0;

    if (quest->completed_)
    {
        exp += quest->experience_points_;
    }

    for (Quest* subquests : quest->subquests_)
    {
        exp += calculatePathwayExperience(subquests);
    }

    return exp;
}

/**
    @param: A string reference to a filter with a default value of "NONE".
    @post: With default filter "NONE": Print out every quest in the list.
           With filter "COMPLETE":   Only print out the completed quests in the list.
           With filter "INCOMPLETE": Only print out the incomplete quests in the list.
           With filter "AVAILABLE":  Only print out the available quests in the list.
           If an invalid filter is passed, print "Invalid Filter\n"
    Printing quests should be of the form:
    [Quest title]: [Complete / Not Complete]  
    [Quest description]\n
*/

void QuestList::questHistory(const string& filter) const
{
    Node<Quest*>* ptr = getHeadNode();
    
    while(ptr)
    {
        if (filter == "COMPLETE")
        {
            if(ptr->getItem()->completed_ == true)
            {
                printQuest(ptr->getItem());
            }
        }
        
        else if (filter == "INCOMPLETE")
        {
            if(ptr->getItem()->completed_ == false)
            {
                printQuest(ptr->getItem());
            }
        }

        else if (filter == "AVAILABLE")
        {
            if(questAvailable(ptr->getItem()))
            {
                printQuest(ptr->getItem());
            }
        }

        else if(filter == "NONE")
        {
            printQuest(ptr->getItem());
        }
        else
        {
            cout<<"Invalid Filter\n";
        }
        ptr = ptr->getNext();
    }
}

/**
    @param: A quest pointer to a main quest
    @post:  Outputs subquest pathway. Print quest names with two spaces ("  ") of indentation for each subquest, recursively.
            Also print the percentage of experience gained in this pathway, rounded down to the lower integer.
            The format should be of the form:
            [Main Quest] ([Pathway XP] / [Projected XP]% Complete)
                [Subquest0]: [Complete / Not Complete]
                    [Sub-Subquest01]: [Complete / Not Complete]
                        [Sub-Subquest011]: [Complete / Not Complete]
                    [Subquest02]: [Complete / Not Complete]
            Hint: You can write a helper function to print the subquests recursively. If the given quest is already marked as completed, you can assume that all of its subquests are also completed.
*/

void QuestList::printQuestDetails(Quest* const& quest) const
{
    int path_experience = calculatePathwayExperience(quest);
    int projected_experience = calculateProjectedExperience(quest);
    int completion_percentage = (projected_experience > 0) ? (path_experience * 100 / projected_experience) : 0;

    cout<<quest->title_<<" ("<<completion_percentage<<"% Complete)"<<endl;
    helperQuestDetails(quest,0);
    cout<<"\n";

}


/**
    @param: A Quest pointer and integer value
    @return: Until the quest pointer is null, the function will print the subquests and whether it is complete or not while also calling the helper function for spaces to indent it
*/
void QuestList::helperQuestDetails(Quest* const& quest, int val) const {
    
    if(!quest)
    {
        return;
    }

    for (auto& subquests : quest->subquests_)
    {
        helperSpaces(val +2);
        cout<<subquests->title_<<": "<< (subquests->completed_ ? "Complete" : "Not Complete")<<endl;
        helperQuestDetails(subquests, val + 2);
    }
}


/**
    @param: A reference to integer value
    @return: Helps print out spaces to indent the subquests
*/

void QuestList::helperSpaces(const int& val) const {
    for (int i=0;i<val;i++)
    {
        cout<<" ";
    }
}