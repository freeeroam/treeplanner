#include <iostream>
#include <fstream>
#include <signal.h>
#include <curses.h>
#include <cmath>
#include "treeplanner.h"

Item * root;
std::string items_file_name;
std::list <Item *> all_items;

int main()
{
  items_file_name = "data/items.txt";
  root = initialise_root_item();
  Item * current_item = root;
  std::list <Item *> ::const_iterator selected_item;
  std::list <Item *> ::const_iterator first_visible_item;
  int input_char = 0;
  int max_rows = 0;

  initscr();
  noecho();
  cbreak();
  curs_set(0);
  keypad(stdscr, true);
  load_all(items_file_name);
  selected_item = root->get_children().begin();
  first_visible_item = selected_item;

  while (true)
  {
    max_rows = display_item_view(current_item, *selected_item,
                                 first_visible_item);
    display_help_bar();
    refresh();

    if ((input_char = getch()) == 'q') // quit
    {
      close_program();
      break;
    } else if (input_char == 'r') // refresh
    {
      erase();
      continue;
    } else if (input_char == 's') // save
    {
      save_all(items_file_name);
    } else if (input_char == 'a') // add new
    {
      if (add_new_item(*current_item) &&
          current_item->get_children().size() == 1)
      {
        selected_item = current_item->get_children().begin();
        first_visible_item = selected_item;
      } // if
    } else if (input_char == 'd') // delete
    {
      if (selected_item != current_item->get_children().end() &&
          confirm_delete())
      {
        delete_item(*selected_item);
        selected_item = current_item->get_children().begin();
        first_visible_item = current_item->get_children().begin();
      } // if
      erase();
    } else if (input_char == '?') // help
    {
      display_help_view();
    } else if (input_char == 'j' || input_char == KEY_UP) // up
    {
      if (selected_item == current_item->get_children().begin())
      {
        selected_item = current_item->get_children().end();
        selected_item--;
      } else
      {
        selected_item--;
      } // else

      // Scroll up or down if selected item is not visible
      if (first_visible_item != current_item->get_children().begin() &&
          before(current_item->get_children(), selected_item,
                        first_visible_item))
      {
          first_visible_item--;
      } else if (std::distance(first_visible_item, selected_item) >= max_rows)
      {
        first_visible_item =
          std::next(first_visible_item,
                    std::distance(first_visible_item,
                                  selected_item) - max_rows + 1);
      } // else if
      erase();
    } else if (input_char == 'k' || input_char == KEY_DOWN) // down
    {
      if (selected_item == std::prev(current_item->get_children().end()))
      {
        selected_item = current_item->get_children().begin();
      } else
      {
        selected_item++;
      } // else

      // Scroll up or down if selected item is not visible
      if (selected_item == current_item->get_children().begin() &&
          before(current_item->get_children(), selected_item,
                 first_visible_item))
      {
        first_visible_item = selected_item;
      } else if (std::distance(first_visible_item, selected_item) >= max_rows)
      {
        first_visible_item++;
      } // if
      erase();
    } else if ((input_char == 'h' || input_char == KEY_LEFT) &&
               current_item != root) // back
    {
      selected_item =
        iterator_at(current_item,
                    &(current_item->get_parent()->get_children()));
      first_visible_item =
        current_item->get_parent()->get_children().begin();
      current_item = current_item->get_parent();
      erase();
    } else if ((input_char == 'l' || input_char == '\n' ||
                input_char == KEY_RIGHT || input_char == KEY_ENTER) &&
               !current_item->get_children().empty()) // view item
    {
      current_item = *selected_item;
      selected_item = current_item->get_children().begin();
      first_visible_item = selected_item;
      erase();
    } else if (input_char == 'g') // first item
    {
      selected_item = current_item->get_children().begin();
      first_visible_item = selected_item;
      erase();
    } else if (input_char == 'G') // last item
    {
      selected_item = std::prev(current_item->get_children().end());
      if (std::distance(first_visible_item, selected_item) >= max_rows)
      {
        first_visible_item =
          std::next(first_visible_item,
                    std::distance(first_visible_item, selected_item) -
                    max_rows + 1);
      } // if
    } // if
    refresh();
  } // while

  return 0;
} // function main

// Creates an Item instance, initialises it with root info and returns a
// pointer to it.
Item * initialise_root_item()
{
  UniqueId root_id;
  root_id.id = 0;
  std::string root_name = "Root";
  std::string root_desc = "Ancestor of all items.";
  Item * root_item = new Item(root_id, root_name, root_desc);
  all_items.push_back(root_item);
  return root_item;
} // function initialise_root_item

// Display help information at the bottom of the window.
void display_help_bar()
{
  move(LINES - 1, 1);
  addstr("? - help");
  refresh();
} // function display_help_bar

// Display the main command keys, until the user presses any key.
void display_help_view()
{
  erase();
  addstr(" q - quit\n a - add item\n d - delete item\n s - save\n"
         " r - refresh\n g - top item\n G - bottom item\n\n"
         "PRESS ANY KEY TO CONTINUE");
  while (!getch())
  {} // while
  erase();
} // function display_help_view

// Displays information about the item given as the first argument and
// lists its children/dependencies.
int display_item_view(Item * item, Item * selected_item,
                      std::list <Item *> ::const_iterator first_visible)
{
  std::list <Item *> ::const_iterator it;
  int item_count = 0;
  int border_len = (COLS >= 80) ? 70 : COLS - 1;
  int starting_row = 0;
  int max_rows = 0;

  if (item == root)
  {
    starting_row = 2;
    move(1,1);
    printw("Total items: %d", all_items.size() - 1);
  } else
  {
    starting_row = 4;
    move(1, 1);
    print_bold("Name/Title: ");
    addstr(item->get_name().c_str());
    move(3, 1);
    print_bold("Description: ");
    addstr(item->get_content().c_str());
  } // else

  // Find number of rows that can fit between the item details and the
  // input/help bar at the bottom of the screen
  max_rows = std::floor((LINES - starting_row - 6) / 2);

  mvhline(starting_row + 1, 1, ACS_HLINE, border_len);
  for (it = first_visible; it != item->get_children().end(); it++)
  {
    if (++item_count > max_rows)
    {
      break;
    } // if
    move(2 * item_count + starting_row, 3);
    display_item_row(*it, selected_item);
    mvaddch(2 * item_count + starting_row, 1, ACS_VLINE);
    mvhline(2 * item_count + starting_row + 1, 1, ACS_HLINE, border_len);
  } // for
  refresh();
  return max_rows;
} // function display_item_page

// Prints the name of the given item in a style based on whether the item is
// selected or not.
void display_item_row(Item * item, Item * selected_item)
{
  int char_index;
  int cursor_y = 0;
  int cursor_x = 0;

  getyx(stdscr, cursor_y, cursor_x);
  if (item == selected_item)
  {
    for (char_index = 0; char_index < item->get_name().size(); char_index++)
    {
      if (cursor_x + char_index > COLS - 1 ||
          cursor_x + char_index > 70 - 1)
      {
        break;
      } // if
      addch(item->get_name()[char_index] | A_STANDOUT);
    } // for
  } else
  {
    for (char_index = 0; char_index < item->get_name().size(); char_index++)
    {
      if (cursor_x + char_index > COLS - 1 ||
          cursor_x + char_index > 70 - 1)
      {
        break;
      } // if
      addch(item->get_name()[char_index]);
    } // for
  } // else
  refresh();
} // function display_item_row

// Asks the user to enter item information and attempts to create a new item,
// returning true if succesful and false otherwise.
bool add_new_item(Item & parent)
{
  UniqueId item_id;
  std::string item_name = get_string_input("Enter name/title: ");
  std::string item_desc = "";
  if (!item_name.empty())
  {
    item_desc = get_string_input("Enter description: ");
    all_items.push_back(create_item(item_id, parent.get_id(), item_name,
                                    item_desc));
    return true;
  } // if
  return false;
} // function add_new_item

// Prompts the user for input and returns the entered string, or an empty
// string if the user cancels the operation.
std::string get_string_input(std::string prompt)
{
  std::string input_str = "";
  int input_char = 0;
  int cursor_y = 0;
  int cursor_x = 0;

  mvaddstr(LINES - 4, 1, prompt.c_str());
  while((input_char = getch()) != '\n' ||
        input_char == KEY_ENTER)
  {
    refresh();
    if (input_char == 27) // ESC
    {
      clear_input_bar();
      return "";
    } else if (input_char == KEY_BACKSPACE && !input_str.empty())
    {
      getyx(stdscr, cursor_y, cursor_x);
      mvaddch(cursor_y, cursor_x - 1, ' ');
      move(cursor_y, cursor_x - 1);
      input_str.pop_back();
      continue;
    } else if (input_char == KEY_BACKSPACE)
    {
      continue;
    } // else if
    echochar(input_char);
    input_str.push_back(input_char);
  } // while
  clear_input_bar();
  return input_str;
} // function get_string input

// Clears the rest of the line, starting from the given coordinate.
void clear_line(int y, int x)
{
  move(y, x);
  hline(' ', COLS - x);
  refresh();
} // function clear_line

// Clear input bar and reset the help bar.
void clear_input_bar()
{
  for (int line_count = 1; line_count < 5; line_count++)
  {
    clear_line(LINES - line_count, 0);
  } // for
  display_help_bar();
  refresh();
} // function clear_input_bar

// Delete the given item and its children recursively.
void delete_item(Item * item)
{
  std::list <Item *> ::const_iterator it;
  item->get_parent()->remove_child(item);
  for (it = all_items.begin(); it != all_items.end(); it++)
  {
    if (*it == item)
    {
      refresh();
      all_items.erase(it);
      break;
    } // if
  } // for

  // Delete the item's children
  for (it = item->get_children().begin(); it != item->get_children().end();)
  {
    delete_item(*it++);
  } // for
  delete item;
} // function delete_item

// Prompts the user to confirm the deletion of an item and returns true if the
// user confirms, false otherwise.
bool confirm_delete()
{
  int input_char = 0;
  mvaddstr(LINES - 4, 1,
           "Are you sure you want to delete the selected item? (y/n)");
  refresh();
  if ((input_char = getch()) == 'y' ||
      input_char == 'Y')
  {
    clear_input_bar();
    return true;
  } // if
  clear_input_bar();
  return false;
} // function confirm_delete

// Load all items from the file with the given file name.
void load_all(const std::string & filename)
{
  std::fstream file (filename, std::fstream::in);
  std::string input_line = "";
  std::string name, content;
  std::list <std::string> field_list;
  int item_id_num = 0;
  int parent_id_num = 0;
  UniqueId item_id, parent_id;
  std::string delimiter = "@@@";
  size_t position = 0;
  Item * new_item = nullptr;
  int max_id = 0;
  int line_count = 0;

  while (std::getline(file, input_line))
  {
    position = 0;
    line_count++;
    while ((position = input_line.find(delimiter)) != std::string::npos)
    {
      field_list.push_back(input_line.substr(0, position));
      input_line.erase(0, position + delimiter.size());
    } // while

    item_id_num = std::stoi(field_list.front());
    parent_id_num = std::stoi(field_list.back());
    name = *std::next(field_list.begin(), 1);
    content = *std::next(field_list.begin(), 2);

    if (item_id_num > max_id)
    {
      max_id = item_id_num;
    } // if

    item_id.id = item_id_num;
    parent_id.id = parent_id_num;
    if ((new_item = create_item(item_id, parent_id, name, content)) !=
         nullptr)
    {
      all_items.push_back(new_item);
    } // if
    field_list.clear();
  } // while

  file.close();
  UniqueId::set_next_id(max_id + 1);
} // function load_all

// Performs a linear search of all items and returns a pointer to the item
// with the given ID, or nullptr if no such item exists.
Item * find_item(const UniqueId item_id)
{
  std::list <Item *> ::const_iterator it;
  for (it = all_items.begin(); it != all_items.end(); it++)
  {
    if ((*it)->get_id() == item_id)
    {
      return *it;
    } // if
  } // for
  return nullptr;
} // function find_item

// Save all items to the file with the given file name in a breadth-first fashion.
void save_all(const std::string & filename)
{
  std::fstream file (filename, std::fstream::out);
  std::list <Item *> ::const_iterator it;
  Item * current_item = nullptr;
  std::list <Item *> queue;
  std::string delimiter = "@@@";

  for (it = root->get_children().begin(); it != root->get_children().end();
       it++)
  {
    queue.push_back(*it);
  } // for

  while (!queue.empty())
  {
    current_item = queue.front();
    file << current_item->get_id().id << "@@@"
         << current_item->get_name() << "@@@"
         << current_item->get_content() << "@@@"
         << current_item->get_parent()->get_id().id << "@@@" << std::endl;
    queue.pop_front();

    for (it = current_item->get_children().begin();
         it != current_item->get_children().end(); it++)
    {
      queue.push_back(*it);
    } // for
  } // while
  file.close();
} // function save_all

// Creates a new instance of Item with the given details and returns a
// pointer to it.
Item * create_item(UniqueId item_id, UniqueId parent_id,
                   std::string & name, std::string & content)
{
  Item * new_item = nullptr;
  Item * parent = find_item(parent_id);

  if (parent != nullptr)
  {
    new_item = new Item(item_id, name, content);
    if (parent != nullptr)
    {
      new_item->set_parent(parent);
    } // if
  } // if
  return new_item;
} // function create_item

// Saves all items and closes stdscr.
void close_program()
{
  save_all(items_file_name);
  endwin();
} // function close_program

// Displays the given string in bold.
void print_bold(std::string str)
{
  for (int char_index = 0; char_index < str.size(); char_index++)
  {
    addch(str[char_index] | A_BOLD);
  } // for
} // function print_bold

// Displays the given string bold and underlined.
void print_bold_underlined(std::string str)
{
  for (int char_index = 0; char_index < str.size(); char_index++)
  {
    addch(str[char_index] | A_BOLD | A_UNDERLINE);
  } // for
} // function print_bold_underlined

// Returns an iterator pointing to the given element in the given container.
std::list <Item *> ::const_iterator iterator_at(
    Item * elem, const std::list <Item *> * container)
{
  std::list <Item *> ::const_iterator it;
  for (it = container->begin(); it != container->end(); it++)
  {
    if (elem == *it)
    {
      return it;
    } // if
  } // for
  return container->end();
} // function iterator_at

// Returns true if the item pointed to by the iterator first comes before
// the item pointed to by the iterator second
bool before(std::list <Item *> container,
            std::list <Item *> ::const_iterator first,
            std::list <Item *> ::const_iterator second)
{
  std::list <Item *> ::const_iterator it;
  if (second != container.begin() &&
      first != std::prev(container.end()))
  {
    for (it = std::prev(second); true; it--)
    {
      if (it == first)
      {
        return true;
      } else if (it == container.begin() || it == container.end())
      {
        break;
      } // else if
    } // for
  } // if
  return false;
} // function before
