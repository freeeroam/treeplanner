#include <iostream>
#include "treeplanner.h"

int UniqueId::next_id = 0; // set initial ID

UniqueId::UniqueId()
{
  id = ++next_id;
} // function UniqueId::UniqueId

UniqueId::UniqueId(const UniqueId & original)
{
  id = original.id;
} // function UniqueId::UniqueId

UniqueId & UniqueId::operator=(const UniqueId & original)
{
  id = original.id;
  return (*this);
} // function UniqueId::operator=

bool UniqueId::operator==(const UniqueId & other_id) const
{
  return id == other_id.id;
} // function UniqueId::operator==

bool UniqueId::operator!=(const UniqueId & other_id) const
{
  return id != other_id.id;
} // function UniqueId::operator!=

 void UniqueId::set_next_id(int id)
{
  next_id = id;
} // function set_next_id
