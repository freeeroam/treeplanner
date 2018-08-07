#ifndef TREEPLAN_H
#define TREEPLAN_H
#include <string>
#include <list>
#include <iterator>

// Models a unique identifier used for items.
// This class is used to hide the implementation of the unique IDs
// so it can be changed at a later point.
class UniqueId
{
  protected:
    static int next_id;
  public:
    int id;
    UniqueId();
    UniqueId(const UniqueId & original);
    UniqueId & operator=(const UniqueId & original);
    bool operator==(const UniqueId & other_id) const;
    bool operator!=(const UniqueId & other_id) const;
    static void set_next_id(int id);
}; // class UniqueId

// Models any item in the tree planner.
// This class can be further extended if required.
class Item
{
  private:
    Item * parent; // parent of the item
    UniqueId id; // a unique identifier
    std::string name; // the name of the item
    std::string content; // more information about the item
    std::list <Item *> children; // children (or dependencies)
  public:
    Item(UniqueId item_id, std::string & item_name,
         std::string & item_content);
    Item(std::string & item_name, std::string & item_content);
    Item();
    Item * get_parent() const;
    const UniqueId & get_id() const;
    const std::string & get_name() const;
    const std::string & get_content() const;
    const std::list <Item *> & get_children() const;
    bool set_parent(Item * new_parent);
    bool set_name(const std::string new_name);
    bool set_content(const std::string new_content);
    bool set_children(std::list <Item *> new_children);
    bool add_child(Item * child);
    bool remove_child(Item * child);
    bool remove_child(const UniqueId child_id);
    bool has_child(Item * item);
    bool has_child(UniqueId item_id);
    bool operator==(const Item & other_item) const;
    bool operator!=(const Item & other_item) const;
}; // class Item

// Main function declarations
Item * initialise_root_item();
void update_screen_size(int signal);
void display_help_bar();
void display_help_view();
int display_item_view(Item * item, Item * selected_item,
                      std::list <Item *> ::const_iterator first_visible);
void display_item_row(Item * item, Item * selected_item);
bool add_new_item(Item & parent);
std::string get_string_input(std::string prompt);
void clear_line(int y, int x);
void clear_input_bar();
void delete_item(Item * item);
bool confirm_delete();
void load_all(const std::string & filename);
Item * find_item(const UniqueId item_id);
void save_all(const std::string & filename);
Item * create_item(UniqueId item_id, UniqueId parent_id,
                   std::string & name, std::string & content);
void close_program();
void print_bold(std::string string);
void print_bold_underlined(std::string string);
std::list <Item *> ::const_iterator iterator_at(
    Item * elem, const std::list <Item *> * container);
bool before(std::list <Item *> container,
            std::list <Item *> ::const_iterator first,
            std::list <Item *> ::const_iterator second);

#endif
