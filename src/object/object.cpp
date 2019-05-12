#include "object/object.hpp"

void object::cleanup () {}

object::object  () = default;
object::~object () { this->cleanup(); }