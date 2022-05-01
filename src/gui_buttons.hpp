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

#ifndef HEADER_CONSTRUO_GUI_BUTTONS_HPP
#define HEADER_CONSTRUO_GUI_BUTTONS_HPP

#include <functional>

#include "colors.hpp"
#include "gui_component.hpp"

class GUIButton : public GUIComponent
{
public:
  GUIButton (const std::string& title, int x_pos_, int y_pos_, int width_, int height_);
  GUIButton (const std::string& title);

  void draw_border_hover(GraphicContext*);
  void draw_border_pressed(GraphicContext*);
  void draw_border_normal(GraphicContext*);

  void on_mouse_enter() override;
  void on_mouse_leave() override;

  void on_primary_button_press (int x, int y) override;
  void on_primary_button_release (int x, int y) override;

  void draw (GraphicContext*) override;

  virtual void draw_content (GraphicContext*);
  virtual void on_click ();

protected:
  std::string m_title;
  bool m_mouse_over;
  bool m_pressed;
};

class GUIRunButton : public GUIButton
{
public:
  GUIRunButton ();
  void draw_content (GraphicContext*) override;
  void on_click() override;
};

class GUISlowMoButton : public GUIButton
{
public:
  GUISlowMoButton ();
  void draw_content (GraphicContext*) override;
  void on_click() override;
};

class GUIQuitButton : public GUIButton
{
public:
  GUIQuitButton ();
  void on_click() override;
};

class GUILoadButton : public GUIButton
{
public:
  GUILoadButton ();
  void on_click() override;
};

inline bool always_false()
{
  return false;
}

class GUIGenericButton : public GUIButton
{
public:
  using Func = std::function<void ()>;
  using HighlightFunc = std::function<bool ()>;

public:
  GUIGenericButton (const std::string& title, int x, int y, int width, int height,
                    Func func, HighlightFunc hfunc = always_false) :
    GUIButton(title, x, y, width, height),
    m_func(func),
    m_hfunc(hfunc)
  {
  }

  GUIGenericButton (const std::string& title, Func func, HighlightFunc hfunc = always_false) :
    GUIButton(title),
    m_func(func),
    m_hfunc(hfunc)
  {
  }

  void on_click() override
  {
    m_func();
  }

  void draw_content (GraphicContext* gc) override
  {
    if (m_hfunc()) {
      gc->draw_fill_rect(static_cast<float>(m_x),
                         static_cast<float>(m_y),
                         static_cast<float>(m_x + m_width),
                         static_cast<float>(m_y + m_height),
                         Colors::button_bg_active);
    }

    GUIButton::draw_content (gc);
  }

private:
  Func m_func;
  HighlightFunc m_hfunc;
};

#endif

/* EOF */
