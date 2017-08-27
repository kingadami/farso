/* 
  Farso: a simple GUI.
  Copyright (C) DNTeam <dnt@dnteam.org>
 
  This file is part of Farso.
 
  Farso is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Farso is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with Farso.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "widgetjsonparser.h"

#if FARSO_HAS_RAPIDJSON == 1

#include "controller.h"
#include <kobold/log.h>
using namespace Farso;

/***********************************************************************
 *                             Constructor                             *
 ***********************************************************************/
WidgetJsonParser::WidgetJsonParser()
{
}

/***********************************************************************
 *                              Destructor                             *
 ***********************************************************************/
WidgetJsonParser::~WidgetJsonParser()
{
}

/***********************************************************************
 *                             loadFromJson                            *
 ***********************************************************************/
bool WidgetJsonParser::loadFromJson(const Kobold::String& jsonStr)
{
   bool res = true;
   rapidjson::Document doc;
   doc.Parse(jsonStr.c_str());
   if(doc.HasParseError())
   {
      Kobold::Log::add(Kobold::Log::LOG_LEVEL_ERROR,
            "Error: tried to add widgets from an invalid JSON string!");
      return false;
   }
   rapidjson::Value::ConstMemberIterator itor = doc.FindMember("widget");
   while((itor != doc.MemberEnd()) && (res))
   {
      res |= parseJsonWidget(itor->value, NULL);
      ++itor;
   }

   return res;

}

/***********************************************************************
 *                              parseInt                               *
 ***********************************************************************/
int WidgetJsonParser::parseInt(const rapidjson::Value& value, 
      const Kobold::String& name, int defaultValue)
{
   rapidjson::Value::ConstMemberIterator it = value.FindMember(name.c_str());
   if(it != value.MemberEnd() && it->value.IsInt())
   {
      return it->value.GetInt();
   }

   return defaultValue;
}

/***********************************************************************
 *                             parseFloat                              *
 ***********************************************************************/
float WidgetJsonParser::parseFloat(const rapidjson::Value& value, 
      const Kobold::String& name, float defaultValue)
{
   rapidjson::Value::ConstMemberIterator it = value.FindMember(name.c_str());
   if(it != value.MemberEnd() && it->value.IsFloat())
   {
      return it->value.GetFloat();
   }

   return defaultValue;
}

/***********************************************************************
 *                            parseBoolean                             *
 ***********************************************************************/
bool WidgetJsonParser::parseBoolean(const rapidjson::Value& value, 
      const Kobold::String& name, bool defaultValue)
{
   rapidjson::Value::ConstMemberIterator it = value.FindMember(name.c_str());
   if(it != value.MemberEnd() && it->value.IsBool())
   {
      return it->value.GetBool();
   }

   return defaultValue;
}

/***********************************************************************
 *                             parseString                             *
 ***********************************************************************/
Kobold::String WidgetJsonParser::parseString(const rapidjson::Value& value,
      const Kobold::String& name)
{
   rapidjson::Value::ConstMemberIterator it = value.FindMember(name.c_str());
   if(it != value.MemberEnd() && it->value.IsString())
   {
      return it->value.GetString();
   }

   return "";
}

/***********************************************************************
 *                             parseVector2                            *
 ***********************************************************************/
Ogre::Vector2 WidgetJsonParser::parseVector2(const rapidjson::Value& value,
      const Kobold::String& name)
{
   rapidjson::Value::ConstMemberIterator it = value.FindMember(name.c_str());
   if(it != value.MemberEnd() && it->value.IsArray() && it->value.Size() == 2 &&
      it->value[0].IsNumber() && it->value[1].IsNumber())
   {
      return Ogre::Vector2(it->value[0].GetFloat(), it->value[1].GetFloat());
   }

   return Ogre::Vector2(0.0f, 0.0f);
}

/***********************************************************************
 *                               parseRect                             *
 ***********************************************************************/
Rect WidgetJsonParser::parseRect(const rapidjson::Value& value,
      const Kobold::String& name)
{
   Rect rect;

   rapidjson::Value::ConstMemberIterator it = value.FindMember(name.c_str());
   if(it != value.MemberEnd() && it->value.IsArray() && 
      it->value.Size() == 4 && it->value[0].IsInt() &&
      it->value[1].IsInt() && it->value[2].IsInt() && it->value[3].IsInt())
   {
      rect.set(it->value[0].GetInt(), it->value[1].GetInt(), 
            it->value[2].GetInt(), it->value[3].GetInt());
   }

   return rect;
}

/***********************************************************************
 *                              parseColor                             *
 ***********************************************************************/
Color WidgetJsonParser::parseColor(const rapidjson::Value& value,
      const Kobold::String& name, bool& defined)
{
   Color color;
   defined = false;

   rapidjson::Value::ConstMemberIterator it = value.FindMember(name.c_str());
   if(it != value.MemberEnd() && it->value.IsArray() && 
      it->value.Size() == 4 && it->value[0].IsInt() &&
      it->value[1].IsInt() && it->value[2].IsInt() && it->value[3].IsInt())
   {
      defined = true;
      color.red = it->value[0].GetInt();
      color.green = it->value[1].GetInt();
      color.blue = it->value[2].GetInt();
      color.alpha = it->value[3].GetInt();
   }

   return color;
}

/***********************************************************************
 *                            parseFontInfo                            *
 ***********************************************************************/
WidgetJsonParser::FontInfo WidgetJsonParser::parseFontInfo(
      const rapidjson::Value& value, const Kobold::String& name,
      Font::Alignment defaultAlign)
{
   rapidjson::Value::ConstMemberIterator it = value.FindMember(name.c_str());
   FontInfo res;
   res.filename = "";
   res.size = 0;
   res.alignment = defaultAlign;

   if(it != value.MemberEnd() && it->value.IsArray()) 
   {
      if(it->value.Size() > 0 && it->value[0].IsString())
      {
         res.filename = it->value[0].GetString();
      }
      if(it->value.Size() > 1 && it->value[1].IsInt())
      {
         res.size = it->value[1].GetInt();
      }
      if(it->value.Size() > 2 && it->value[2].IsString())
      {
         Kobold::String align = it->value[2].GetString();
         if(align == "left")
         {
            res.alignment = Font::TEXT_LEFT;
         }
         else if(align == "right")
         {
            res.alignment = Font::TEXT_RIGHT;
         }
         else
         {
            res.alignment = Font::TEXT_CENTERED;
         }
      }
   }

   return res;

}

/***********************************************************************
 *                             parseWindow                             *
 ***********************************************************************/
Widget* WidgetJsonParser::parseWindow(const rapidjson::Value& value,
      Widget* parent)
{
   /* Check if no parent */
   if(parent != NULL)
   {
      Kobold::Log::add(Kobold::Log::LOG_LEVEL_ERROR,
            "Error: A window should have no parent.");
      return NULL;
   }
   /* Get window parameters */
   bool modal = parseBoolean(value, "modal", false);

   /* Create it */
   Window* window = new Window(size.x, size.y, caption);
   if(modal)
   {
      window->setModal();
   }

   return window;
}

/***********************************************************************
 *                             parseButton                             *
 ***********************************************************************/
Widget* WidgetJsonParser::parseButton(const rapidjson::Value& value,
      Widget* parent)
{
   /* Parse its own options */
   bool decored = parseBoolean(value, "decored", true);
   bool rounded = parseBoolean(value, "rounded", false);
   FontInfo font = parseFontInfo(value, "font", Font::TEXT_CENTERED);
   //TODO: parse and set Menu.
   //TODO: parse skin types.

   /* Create and set the button */
   Button* button = new Button(pos.x, pos.y, size.x, size.y, caption,
         parent);
   if(!decored)
   {
      button->setWithoutDecorations();
   }
   if(rounded)
   {
      button->setRoundedEdges();
   }
   if(!font.filename.empty() && font.size != 0)
   {
      button->setFont(font.filename, font.size);
   }
   return button;
}

/***********************************************************************
 *                            parseCheckBox                            *
 ***********************************************************************/
Widget* WidgetJsonParser::parseCheckBox(const rapidjson::Value& value,
      Widget* parent)
{
   /* Parse checkbox fields and create it */
   bool checked = parseBoolean(value, "checked", false);
   return new CheckBox(pos.x, pos.y, size.x, caption, checked, parent);
}

/***********************************************************************
 *                        parseClickablePicture                        *
 ***********************************************************************/
Widget* WidgetJsonParser::parseClickablePicture(const rapidjson::Value& value,
      Widget* parent)
{
   Kobold::String filename = parseString(value, "filename");
   if(filename.empty())
   {
      Kobold::Log::add(Kobold::Log::LOG_LEVEL_ERROR,
            "Error: A clickable picture must have its image defined.");
      return NULL;
   }
   return new ClickablePicture(pos.x, pos.y, filename, parent);
}

/***********************************************************************
 *                           parseContainer                            *
 ***********************************************************************/
Widget* WidgetJsonParser::parseContainer(const rapidjson::Value& value,
      Widget* parent)
{
   /* Parse container fields */
   bool filled = parseBoolean(value, "filled", false);
   Rect rect = parseRect(value, "rect");
   Container::ContainerType contType = Container::TYPE_TOP_LEFT;
   Kobold::String typeStr = parseString(value, "containerType");
   if(typeStr == "topLeft")
   {
      contType = Container::TYPE_TOP_LEFT;
   }
   else if(typeStr == "topRight")
   {
      contType = Container::TYPE_TOP_RIGHT;
   }
   else if(typeStr == "topCentered")
   {
      contType = Container::TYPE_TOP_CENTERED;
   }
   else if(typeStr == "bottomLeft")
   {
      contType = Container::TYPE_BOTTOM_LEFT;
   }
   else if(typeStr == "bottomRight")
   {
      contType = Container::TYPE_BOTTOM_RIGHT;
   }
   else if(typeStr == "bottomCentered")
   {
      contType = Container::TYPE_BOTTOM_CENTERED;
   }

   /* Create and define it */
   Container* container;
   if(rect.isDefined())
   {
      container = new Container(contType, rect, parent);
   }
   else if(size.x != 0 && size.y != 0)
   {
      container = new Container(contType, pos.x, pos.y, 
            size.x, size.y, parent);
   }
   else
   {
      container = new Container(contType, parent);
   }

   if(filled)
   {
      container->setFilled();
   }

   return container;
}

/***********************************************************************
 *                           parseFileSelector                         *
 ***********************************************************************/
Widget* WidgetJsonParser::parseFileSelector(const rapidjson::Value& value,
      Widget* parent)
{
   /* Parse file selector options */
   bool load = parseBoolean(value, "load", true);
   Kobold::String dir = parseString(value, "directory");
   bool showDirs = parseBoolean(value, "showDirectories", true);
   Kobold::String filter = parseString(value, "filter");

   FileSelector* fileSel = new FileSelector(load, dir, showDirs, parent);
   if(!filter.empty())
   {
      fileSel->setFilter(filter);
   }

   return fileSel;
}

/***********************************************************************
 *                              parseGrid                              *
 ***********************************************************************/
Widget* WidgetJsonParser::parseGrid(const rapidjson::Value& value,
      Widget* parent)
{
   //TODO
   return NULL;
}

/***********************************************************************
 *                              parseLabel                             *
 ***********************************************************************/
Widget* WidgetJsonParser::parseLabel(const rapidjson::Value& value,
      Widget* parent)
{
   /* Parse extra label info */
   FontInfo fontInfo = parseFontInfo(value, "font", Font::TEXT_LEFT);
   bool colorDefined = false;
   Color fontColor = parseColor(value, "fontColor", colorDefined);
   int outline = parseInt(value, "fontOutline", 0);
   bool outlineDefined = false;
   Color outlineColor = parseColor(value, "fontOutlineColor", 
         outlineDefined);
   bool useBorder = parseBoolean(value, "border", false);
   bool breakOnSpace = parseBoolean(value, "breakOnSpace", false);

   /* Create and set the label */
   Label* label = new Label(pos.x, pos.y, size.x, size.y, 
         caption, parent);
   if(!fontInfo.filename.empty())
   {
      label->setFont(fontInfo.filename);
   }
   if(fontInfo.size > 0)
   {
      label->setFontSize(fontInfo.size);
   }
   label->setFontAlignment(fontInfo.alignment);
   if(colorDefined)
   {
      label->setFontColor(fontColor);
   }
   if(outline > 0)
   {
      label->setFontOutline(outline, outlineColor);
   }
   if(useBorder)
   {
      label->enableBorder();
   }
   if(breakOnSpace)
   {
      label->enableBreakLineOnSpace();
   }

   return label;
}

/***********************************************************************
 *                        parseLabelledPicture                         *
 ***********************************************************************/
Widget* WidgetJsonParser::parseLabelledPicture(const rapidjson::Value& value,
      Widget* parent)
{
   /* Parse its elements */
   Kobold::String filename = parseString(value, "filename");
   if(filename.empty())
   {
      Kobold::Log::add(Kobold::Log::LOG_LEVEL_ERROR,
            "Error: A labelled picture must have its image defined.");
      return NULL;
   }
   FontInfo fontInfo = parseFontInfo(value, "font", Font::TEXT_CENTERED);
   bool colorDefined = false;
   Color fontColor = parseColor(value, "fontColor", colorDefined);
   int outline = parseInt(value, "fontOutline", 0);
   bool outlineDefined = false;
   Color outlineColor = parseColor(value, "fontOutlineColor", 
         outlineDefined);

   LabelledPicture* labelledPicture = new LabelledPicture(pos.x, pos.y,
         size.x, size.y, caption, filename, fontColor, outlineColor,
         outline, parent);
   if(!fontInfo.filename.empty())
   {
      labelledPicture->setFont(fontInfo.filename);
   }
   if(fontInfo.size > 0)
   {
      labelledPicture->setFontSize(fontInfo.size);
   }
   labelledPicture->setFontAlignment(fontInfo.alignment);

   return labelledPicture;
}

/***********************************************************************
 *                              parseMenu                              *
 ***********************************************************************/
Widget* WidgetJsonParser::parseMenu(const rapidjson::Value& value,
      Widget* parent)
{
   //TODO
   return NULL;
}

/***********************************************************************
 *                            parsePicture                             *
 ***********************************************************************/
Widget* WidgetJsonParser::parsePicture(const rapidjson::Value& value,
      Widget* parent)
{
   Picture* picture;
   Kobold::String filename = parseString(value, "filename");
   if(filename.empty())
   {
      if(size.x > 0 && size.y > 0)
      {
         picture = new Picture(pos.x, pos.y, size.x, size.y, parent);
      }
      else
      {
         Kobold::Log::add(Kobold::Log::LOG_LEVEL_ERROR,
            "Error: A picture must define either its filename or its size.");
         return NULL;
      }
   }
   else
   {
      picture = new Picture(pos.x, pos.y, filename, parent);
   }
   return picture;
}

/***********************************************************************
 *                          parseProgressBar                           *
 ***********************************************************************/
Widget* WidgetJsonParser::parseProgressBar(const rapidjson::Value& value,
      Widget* parent)
{
   int maxValue = parseInt(value, "maxValue", 100);
   int val = parseInt(value, "value", 0);

   ProgressBar* pb = new ProgressBar(pos.x, pos.y, size.x, size.y, parent);
   pb->setValue(val);
   pb->setMaxValue(maxValue);

   return pb;
}

/***********************************************************************
 *                           parseScrollBar                            *
 ***********************************************************************/
Widget* WidgetJsonParser::parseScrollBar(const rapidjson::Value& value,
      Widget* parent)
{
   Kobold::String strType = parseString(value, "scrollType");
   ScrollBar::ScrollType scType;
   if(strType == "horizontal")
   {
      scType = ScrollBar::TYPE_HORIZONTAL;
   }
   else
   {
      scType = ScrollBar::TYPE_VERTICAL;
   }
   int size = parseInt(value, "size", 100);
   int maxDisplayed = parseInt(value, "maxDisplayed", 1);
   int total = parseInt(value, "total", 1);
   int initial = parseInt(value, "initial", 0);

   ScrollBar* sb = new ScrollBar(scType, pos.x, pos.y, size, parent);
   sb->setTotals(maxDisplayed, total, initial);

   return sb;
}

/***********************************************************************
 *                           parseScrollText                           *
 ***********************************************************************/
Widget* WidgetJsonParser::parseScrollText(const rapidjson::Value& value,
      Widget* parent)
{
   //TODO
   return NULL;
}

/***********************************************************************
 *                              parseSpin                              *
 ***********************************************************************/
Widget* WidgetJsonParser::parseSpin(const rapidjson::Value& value,
      Widget* parent)
{
   Kobold::String spType = parseString(value, "valueType");
   Spin::ValueType valueType = Spin::VALUE_TYPE_INTEGER;
   if(spType == "float")
   {
      valueType = Spin::VALUE_TYPE_FLOAT;
   }
   Spin* sp = new Spin(valueType, pos.x, pos.y, parent);

   bool accelerate = parseBoolean(value, "accelerate", true);
   float val = parseFloat(value, "value", 0.0f);
   Ogre::Vector2 range = parseVector2(value, "range");

   if(valueType == Spin::VALUE_TYPE_INTEGER)
   {
      int delta = parseInt(value, "delta", 1);
      sp->setIntDelta(delta, accelerate);
   }
   else
   {
      float delta = parseFloat(value, "delta", 0.5f);
      sp->setFloatDelta(delta, accelerate);
   }
   sp->setValue(val);
   if(range.x != 0.0f || range.y != 0.0f)
   {
      sp->setRange(range.x, range.y);
   }

   return sp;
}

/***********************************************************************
 *                            parseStackTab                            *
 ***********************************************************************/
Widget* WidgetJsonParser::parseStackTab(const rapidjson::Value& value,
      Widget* parent)
{
   //TODO
   return NULL;
}

/***********************************************************************
 *                            parseTreeView                            *
 ***********************************************************************/
Widget* WidgetJsonParser::parseTreeView(const rapidjson::Value& value,
      Widget* parent)
{
   //TODO
   return NULL;
}

/***********************************************************************
 *                          parseTextSelector                          *
 ***********************************************************************/
Widget* WidgetJsonParser::parseTextSelector(const rapidjson::Value& value,
      Widget* parent)
{
   //TODO
   return NULL;
}

/***********************************************************************
 *                            parseTextEntry                            *
 ***********************************************************************/
Widget* WidgetJsonParser::parseTextEntry(const rapidjson::Value& value,
      Widget* parent)
{
   TextEntry* te = new TextEntry(pos.x, pos.y, size.x, size.y, parent);
   te->setCaption(caption);

   return te;
}

/***********************************************************************
 *                           parseJsonWidget                           *
 ***********************************************************************/
bool WidgetJsonParser::parseJsonWidget(const rapidjson::Value& value, 
      Widget* parent)
{
   if(value.IsObject())
   {
      /* Check its type */
      Kobold::String type = parseString(value, "type");

      /* Parse common widget values */
      id = parseString(value, "id");
      caption = parseString(value, "caption");
      mouseHint = parseString(value, "mouseHint");
      available = parseBoolean(value, "available", true);
      pos = parseVector2(value, "position");
      size = parseVector2(value, "size");

      if((size.x == 0.0f) || (size.y == 0.0f))
      {
         Kobold::Log::add(Kobold::Log::LOG_LEVEL_ERROR,
            "Error: Tried to define a widget with size == 0.0f");
         return false;
      }

      Widget* created = NULL;
      if(type == "window")
      {
         created = parseWindow(value, parent);
      }
      else if(type == "button")
      {
         created = parseButton(value, parent);
      }
      else if(type == "checkbox")
      {
         created = parseCheckBox(value, parent);
      }
      else if(type == "clickablePicture")
      {
         created = parseClickablePicture(value, parent);
      }
      else if(type == "container")
      {
         created = parseContainer(value, parent);
      }
      else if(type == "fileSelector")
      {
         created = parseFileSelector(value, parent);
      }
      else if(type == "grid")
      {
         created = parseGrid(value, parent);
      }
      else if(type == "label")
      {
         created = parseLabel(value, parent);
      }
      else if(type == "labelledPicture")
      {
         created = parseLabelledPicture(value, parent);
      }
      else if(type == "menu")
      {
         created = parseMenu(value, parent);
      }
      else if(type == "picture")
      {
         created = parsePicture(value, parent);
      }
      else if(type == "progressBar")
      {
         created = parseProgressBar(value, parent);
      }
      else if(type == "scrollBar")
      {
         created = parseScrollBar(value, parent);
      }
      else if(type == "scrollText")
      {
         created = parseScrollText(value, parent);
      }
      else if(type == "spin")
      {
         created = parseSpin(value, parent);
      }
      else if(type == "stackTab")
      {
         created = parseStackTab(value, parent);
      }
      else if(type == "textEntry")
      {
         created = parseTextEntry(value, parent);
      }
      else if(type == "textSelector")
      {
         created = parseTextSelector(value, parent);
      }
      else if(type == "treeView")
      {
         created = parseTreeView(value, parent);
      }

      if(!created)
      {
         return false;
      }

      /* Parse and add its children widgets. */
      rapidjson::Value::ConstMemberIterator it = value.FindMember("children");
      if(it != value.MemberEnd() && it->value.IsArray())
      {
         for(size_t child = 0; child < it->value.Size(); child++)
         {
            if(!parseJsonWidget(it->value[child], created))
            {
               /* Error parsing a child. Abort. */
               return false;
            }
         }
      }

      /* Open the window, if just created one */
      if(created->getType() == Widget::WIDGET_TYPE_WINDOW)
      {
         Window* window = static_cast<Window*>(created);
         window->setPosition(pos.x, pos.y);
         window->open();
      }

      /* Disable the widget, if desired */
      if(!available)
      {
         created->disable();
      }

      /* Set its hint */
      if(!mouseHint.empty())
      {
         created->setMouseHint(mouseHint);
      }
   }

   return true;
}

#endif
