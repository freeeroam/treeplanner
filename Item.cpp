#include <iostream>
#include <iterator>
#include <sstream>
#include <fstream>
#include "treeplan.h"

Item::Item(UniqueId item_id, std::string & item_name,
           std::string & item_content)
{
  id = item_id;
  name = item_name;
  content = item_content;
} // function Item::Item

Item::Item(std::string & item_name, std::string & item_content)
{
  UniqueId item_id;
  id = item_id;
  name = item_name;
  content = item_content;
} // function Item::Item

Item::Item()
{
  UniqueId item_id;
  id = item_id;
  name = "Unnamed Item";
  content = "";
} // function Item::Item

Item * Item::get_parent() const
{
  return parent;
} // function Item::get_parent

const UniqueId & Item::get_id() const
{
  return id;
} /// function Item::get_id

const std::string & Item::get_name() const
{
  return name;
} // function Item::get_name

const std::string & Item::get_content() const
{
  return content;
} // function Item::get_content

const std::list <Item *> & Item::get_children() const
{
  return children;
} // function Item::get_children

bool Item::set_parent(Item * new_parent)
{
  if (new_parent != nullptr && new_parent->add_child(this))
  {
    parent = new_parent;
    return true;
  } // if
  return false;
} // function Item::set_parent

bool Item::set_name(const std::string new_name)
{
  if (!new_name.empty())
  {
    name = new_name;
    return true;
  } // if
  return false;
} // function Item::set_name

bool Item::set_content(const std::string new_content)
{
  if (!new_content.empty())
  {
    content = new_content;
    return true;
  } // if
  return false;
} // function Item::set_content

bool Item::set_children(std::list <Item *> new_children)
{
  if (!new_children.empty())
  {
    std::list <Item *> ::iterator it;
    for (it = children.begin(); it != children.end(); it++)
    {
      // add each child by setting itself as its parent
      (*it)->set_parent(this);
    } // for
    return true;
  } // if
  return false;
} // function Item::set_children

bool Item::add_child(Item * child)
{
  if (child != nullptr && !has_child(child))
  {
    children.push_back(child);
    return true;
  } // if
  return false;
} // function Item::add_child

bool Item::remove_child(Item * child)
{
  std::list <Item *> ::iterator it;
  for (it = children.begin(); it != children.end(); it++)
  {
    if (**it == *child)
    {
      children.erase(it);
      return true;
    } // if
  } // for
  return false;
} // function Item::remove_child

bool Item::remove_child(UniqueId child_id)
{
  std::list <Item *> ::iterator it;
  for (it = children.begin(); it != children.end(); it++)
  {
    if ((*it)->get_id() == child_id)
    {
      children.erase(it);
      return true;
    } // if
  } // for
  return false;
} // function Item::remove_child

bool Item::has_child(Item * item)
{
  std::list <Item *> ::iterator it;
  for (it = children.begin(); it != children.end(); it++)
  {
    if (**it == *item)
    {
      return true;
    } // if
  } // for
  return false;
} // function Item::has_child

bool Item::has_child(UniqueId item_id)
{
  std::list <Item *> ::iterator it;
  for (it = children.begin(); it != children.end(); it++)
  {
    if ((*it)->get_id() == item_id)
    {
      return true;
    } // if
  } // for
  return false;
} // function Item::has_child

bool Item::operator==(const Item & other_item) const
{
  return id == other_item.get_id();
} // function Item::operator==

bool Item::operator!=(const Item & other_item) const
{
  return id != other_item.get_id();
} // function Item::operator!=
