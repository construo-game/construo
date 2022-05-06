// Construo - A wire-frame construction game
// Copyright (C) 2002 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef HEADER_LISP_READER_HPP
#define HEADER_LISP_READER_HPP

#include "vector2d.hpp"

#include <prio/reader.hpp>

using ReaderCollection = prio::ReaderCollection;
using ReaderDocument = prio::ReaderDocument;
using ReaderMapping = prio::ReaderMapping;
using ReaderObject = prio::ReaderObject;

namespace prio {

template<> bool read_custom(prio::ReaderMapping const& map, std::string_view key, Vector2d& value_out);

} // namespace prio

#endif

/* EOF */
