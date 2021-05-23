

[gtk2 tutorial](http://www.huzheng.org/ebook/gtk2-tut/x408.html)
[Getting started with gtk3](https://riptutorial.com/gtk3)
[gtkmm tutorial](https://developer.gnome.org/gtkmm-tutorial/stable/sec-widgets-overview.html.zh_CN)

[Gtk4-tutorial](https://github.com/ToshioCP/Gtk4-tutorial)
[using Gtk::Builder in gtkmm](https://riptutorial.com/gtk3/example/19806/-cplusplus--using-gtk--builder-in-gtkmm)

gtkmm 是gtk+的cplusplus封装， 因此可以进行c/c++的混合编程， 它也提供了c++对象与 gtk+原生c对象的转换。具体内容见[basics-gobj-and-wrap](https://developer.gnome.org/gtkmm-tutorial/stable/sec-basics-gobj-and-wrap.html.zh_CN)。因此可以将两方面的结合在一起，在coding时，熟悉哪个的api就用哪个的api。

gtk3与gtk4的xml不兼容，当前glade也仅仅支持gtk3.因此如果gtk4要使用ui，需要gtk4-builder-tool进行转换， [参考](https://discourse.gnome.org/t/difference-xml-between-gtk3-and-gtk4/6066)。 为了使用方便，准备一个脚本"`LD_LIBRARY_PATH=/home/atmel/tools/vcpkg/installed/x64-linux/lib  /home/atmel/tools/vcpkg/installed/x64-linux/tools/gtk/gtk4-builder-tool $@`"


//         auto gdkwin = gtk_widget_get_window(this->m_widget);
//         auto _win = GDK_WINDOW_XID(gdkwin);
//         auto _dpy = GDK_WINDOW_XDISPLAY(gdkwin);


### How to make box containing widgets expand to fill window?

```c++
for (const auto &child : _grid_.get_children()) {
    child->set_hexpand(true);
    child->set_halign(Gtk::ALIGN_FILL);
    child->set_vexpand(true);
    child->set_valign(Gtk::ALIGN_FILL);
}
```

### gtk 与opengl

注意，glad不支持gtk，应采用epoxy.

Gtk::GlArea

### 获取原生句柄

```c++
        /**
         * gtk3 get native win/dpy
         * auto gdkwin = gtk_widget_get_window(widget);
         * auto _win = GDK_WINDOW_XID(gdkwin);
         * auto _dpy = GDK_WINDOW_XDISPLAY(gdkwin);
         **/
        //gtk4, in GtkWindow
        auto surface = this->get_surface();
        auto _win = GDK_SURFACE_XID(surface->gobj()); //if win32 using GDK_SURFACE_HWND
        auto gdkdisplay = this->get_display();
        auto _dpy = GDK_DISPLAY_XDISPLAY(gdkdisplay->gobj());
        //gtkmm4
        inline EGLNativeWindowType _get_XID(const Glib::RefPtr<Gdk::Surface> &surf)
        {
        #if defined(_WIN32) || defined(__CYGWIN__)
        return (EGLNativeWindowType)GDK_SURFACE_HWND(surf->gobj());
        #else
        return GDK_SURFACE_XID(surf->gobj());
        #endif
        }

        inline EGLNativeDisplayType _get_XDISPLAY(const Glib::RefPtr<Gdk::Surface> &surf)
        {
        #if defined(_WIN32) || defined(__CYGWIN__)
        auto hwnd = (EGLNativeWindowType)GDK_SURFACE_HWND (surf->gobj());
        return GetDC ( hwnd );
        //GDK_WIN32_DISPLAY
        #else
        return GDK_SURFACE_XDISPLAY(surf->gobj());
        #endif
        }


```

对类似`DrawingArea`中去获取，采用:
```c++
         auto w = dynamic_cast<Gtk::Widget *>(this)->gobj();
         auto display = gtk_widget_get_display(w);
         auto surface = gtk_native_get_surface(gtk_widget_get_native(w));
         _dpy = gdk_x11_display_get_xdisplay(display);
         _win = GDK_SURFACE_XID(surface);
```
这里需要注意： 由于widget是作为GtKWindow child，因此从widget中取句柄，其实取的是window容器的句柄。比如GtkWindow中包含多个widget，那由这些widget中去获取句柄XID，得到的结果都是一样的。

[THOSE “REALIZE” & “MAP” WIDGET SIGNALS](https://blogs.gnome.org/jnelson/2010/10/13/those-realize-map-widget-signals/)

### libGL error
下面两个问题：
```text
libGL error: No matching fbConfigs or visuals found
libGL error: failed to load driver: swrast
```

通过`export LIBGL_ALWAYS_INDIRECT=1`解决了。

## Gtkmm4交互

### 键盘输入

toplevel普通的场景只要采用下面的方式就可以了
```c++
      auto keyevent = Gtk::EventControllerKey::create();
      keyevent->signal_key_pressed().connect(sigc::mem_fun(*this, &KeyPressExample::on_selfkeypressed), false);
      this->add_controller(keyevent);

   gboolean KeyPressExample::on_selfkeypressed(guint keyval/*is keysym*/, guint keycode, Gdk::ModifierType state)
   {
      auto sym = keyval;
      if (sym == GDK_KEY_Escape || sym == GDK_KEY_q || sym == GDK_KEY_Q)
      ...
      return  GDK_EVENT_STOP;
   }
```

但是实践中需要sub-widget 接收 key event场景，发现sub - widget接收不到keyevent。 如果需要subwidget接收到键盘事件，需要采用下面的方式：
- 首先subwidget还是要和上面的方式一样构造EventControllerKey并添加该controller
- 在toplevel的键盘事件处理函数中，需要增加forward处理，例如
```c++
   gboolean KeyPressExample::on_selfkeypressed(guint keyval, guint keycode, Gdk::ModifierType state)
   {
        return keyevent->forward(*m_drawarea);
   }
```

另外，通常还涉及keysym，kecode之间转换的问题, 如果在X下可以采用下面的方法：
```
      // // XkbKeycodeToKeysym
      // // KeySym sym = XKeycodeToKeysym(this->state.dpy, keyEvent.keycode, 0);
```

### 鼠标移动

```c++
      auto motion = Gtk::EventControllerMotion::create();
      motion->signal_motion().connect(sigc::mem_fun(*this, &mydraw_t::on_selfmotion));
      this->add_controller(motion);
      ...
   void on_selfmotion(double mouse_x, double mouse_y)
   {
       ...
      std::cout << "mouse_x:"<<mouse_x<< " mouse_x:"<<mouse_y << std::endl;
   }

```

### 鼠标按键

使用` Gtk::GestureClick` 这个`Gtk::EventController`派生类，编码方式参考鼠标移动

```c++
      auto button_press = Gtk::GestureClick::create();
      button_press->set_button(GDK_BUTTON_PRIMARY);
      button_press->signal_pressed().connect(sigc::mem_fun(*this, &mydraw_t::on_selfbutton_pressed), false);
      this->add_controller(button_press);
```

### 鼠标drag

使用` Gtk::GestureDrag` 这个`Gtk::EventController`派生类，编码方式参考鼠标移动

```c++
      auto  drag = Gtk::GestureDrag::create();
      drag->set_button(GDK_BUTTON_PRIMARY);
      drag->signal_drag_begin().connect(sigc::mem_fun(*this, &mydraw_t::on_selfdrag_begin),false);
      drag->signal_drag_update().connect(sigc::mem_fun(*this, &mydraw_t::on_selfdrag_update));
      drag->signal_drag_end().connect(sigc::mem_fun(*this, &mydraw_t::on_selfdrag_end), false);
      this->add_controller(drag);
```

###  locale::facet::_S_create_c_locale name not valid

解决方案：在命令行输入 > export LC_ALL="C"

