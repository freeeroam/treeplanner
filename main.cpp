#include <iostream>
#include <fstream>
#include <iterator>
#include <signal.h>
#include <curses.h>
#include <string>
#include <list>
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
  int input_char = 0;

  // Add signal for resizing ************

  initscr();
  noecho();
  cbreak();
  keypad(stdscr, true);
  load_all(items_file_name);
  mvaddstr(1, 1, "success");
  refresh();
  selected_item = root->get_children().begin();


  while (true)
  {
    display_item_view(current_item, *selected_item);
    display_help_bar();
    refresh();

    if ((input_char = getch()) == 'q') // quit
    {
      close_program();
      break;
    } else if (input_char == 's') // save
    {
      save_all(items_file_name);
    } else if (input_char == 'a') // add new
    {
      add_new_item(*current_item);
    } else if (input_char == 'd') // delete
    {
      if (selected_item != current_item->get_children().end() &&
          confirm_delete())
      {
        delete_item(*selected_item);
      } // if
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
        selected_item++;
      } // else
    } else if (input_char == 'k' || input_char == KEY_DOWN) // down
    {
      if (selected_item == std::prev(current_item->get_children().end()))
      {
        selected_item = current_item->get_children().begin();
      } else
      {
        selected_item++;
      } // else
      erase();
    } else if ((input_char == 'h' || input_char == KEY_LEFT) &&
               current_item != root) // back
    {
      selected_item
        = iterator_at(current_item,
                      &(current_item->get_parent()->get_children()));
      current_item = current_item->get_parent();
      erase();
    } else if (input_char == 'l' || input_char == '\n' ||
               input_char == KEY_RIGHT || input_char == KEY_ENTER) // select
    {
      current_item = *selected_item;
      selected_item = current_item->get_children().begin();
      erase();
    } // else if
    refresh();
  } // while

  return 0;
} // function main

Item * initialise_root_item()
{
  UniqueId root_id;
  root_id.id = 0;
  std::string root_name = "Root";
  std::string root_desc = "Ancestor of all items.";
  Item * root_item = new Item(root_id, root_name, root_desc);
  all_items.push_back(root);
  return root;
} // function initialise_root_item

void update_screen_size(int signal)
{
  endwin();
  refresh();
} // function update_screen_size

void display_help_bar()
{
  move(LINES - 4, 1);
  addstr("? - help");
  refresh();
} // function display_help_bar

void display_help_view()
{
  erase();
  addstr("q - quit\n a - add item\n d - delete item\n s - save\n\n"
         "PRESS ANY KEY TO CONTINUE");
  while (!getch())
  {} // while
  erase();
} // function display_help_view

void display_item_view(Item * item, Item * selected_item)
{
  std::list <Item *> ::const_iterator it;
  int line_count = 0;
  int border_len = (COLS >= 80) ? 70 : COLS - 1;

  if (selected_item == root)
  {
    move(1,1);
    printw("Total items: %d", all_items.size() - 1);
  } // if

  mvhline(2, 1, ACS_HLINE, border_len);
  for (it = item->get_children().begin(); it != item->get_children().end();
       it++)
  {
    line_count++;
    move(2 + line_count, 4);
    display_item_row(*it, selected_item);
    mvhline(3 + line_count, 3, ACS_HLINE, border_len);
  } // for
  refresh();
} // function display_item_page

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
      if (cursor_x + char_index > COLS ||
          cursor_x + char_index > 70)
      {
        break;
      } // if
      addch(item->get_name()[char_index] | A_STANDOUT);
      mvaddch(cursor_y, 1, ACS_VLINE);
    } // for
  } else
  {
    for (char_index = 0; char_index < item->get_name().size(); char_index++)
    {
      if (cursor_x + char_index > COLS ||
          cursor_x + char_index > 70)
      {
        break;
      } // if
      addch(item->get_name()[char_index] | A_STANDOUT);
      mvaddch(cursor_y, 1, ACS_VLINE);
    } // for
  } // else
  refresh();
} // function display_item_row

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

std::string get_string_input(std::string prompt)
{
  std::string input_str = "";
  int input_char = 0;
  int cursor_y = 0;
  int cursor_x = 0;

  mvaddstr(LINES - 4, 1, prompt.c_str());
  while((input_char = getch() != '\n' ||
        input_char == KEY_ENTER))
  {
    if (input_char == 27) // ESC
    {
      return "";
    } else if (input_char == KEY_BACKSPACE && !input_str.empty())
    {
      getyx(stdscr, cursor_y, cursor_x);
      mvaddch(cursor_y, cursor_x - 1, ' ');
      input_str.pop_back();
      continue;
    } // else if
    echochar(input_char);
    input_str.push_back(input_char);
  } // while
  return input_str;
} // function get_string input

void clear_line(int y, int x)
{
  move(y, x);
  hline(' ', COLS - x);
  refresh();
} // function clear_line

void clear_input_bar()
{
  for (int line_count = 1; line_count < 5; line_count++)
  {
    clear_line(LINES - line_count, 0);
  } // for
} // function clear_input_bar

void delete_item(Item * item)
{
  std::list <Item *> ::const_iterator it;
  item->get_parent()->remove_child(item);
  for (it = all_items.begin(); it != all_items.end(); it++)
  {
    if (*it == item)
    {
      all_items.erase(it);
    } // if
  } // for
  delete item;
} // function delete_item

bool confirm_delete()
{
  int input_char = 0;
  mvaddstr(LINES - 4, 1,
           "Are you sure you want to delete the selected item? (y/n)");
  refresh();
  if ((input_char == getch()) == 'y' ||
      input_char == 'Y')
  {
    return true;
  } // if
  return false;
} // function confirm_delete

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

    mvaddstr(line_count, 0, "success"); 
    refresh();

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
} // function load_all

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
         << current_item->get_parent()->get_id().id << "@@@";
    queue.pop_front();

    for (it = current_item->get_children().begin();
         it != current_item->get_children().end(); it++)
    {
      queue.push_back(*it);
    } // for
  } // while
  file.close();
} // function save_all

Item * create_item(UniqueId item_id, UniqueId parent_id,
                   std::string & name, std::string & content)
{
  Item * new_item = nullptr;
  Item * parent = find_item(parent_id);

  if (parent != nullptr)
  {
    new_item = new Item(item_id, name, content);
    if (parent_id.id != 0 && parent != nullptr)
    {
      new_item->set_parent(parent);
    } // if
  } // if
  return new_item;
} // function create_item

void close_program()
{
  save_all(items_file_name);
  endwin();
} // function close_program

void print_bold(std::string str)
{
  for (int char_index = 0; char_index < str.size(); char_index++)
  {
    addch(str[char_index] | A_UNDERLINE);
  } // for
} // function print_bold

void print_bold_underlined(std::string str)
{
  for (int char_index = 0; char_index < str.size(); char_index++)
  {
    addch(str[char_index] | A_BOLD | A_UNDERLINE);
  } // for
} // function print_bold_underlined

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
