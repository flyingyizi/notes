

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

### Widgets Without X-Windows

下面这些是不与X-window关联的，因此无法接收event。 有个变通方式是将它们放进Gtk::EventBox container里，通过Gtk::EventBox来接收event：
`Gtk::Alignment,Gtk::Arrow,Gtk::Bin,Gtk::Box,Gtk::Button,Gtk::CheckButton,Gtk::Fixed,Gtk::Image,Gtk::Item,Gtk::Label,Gtk::MenuItem,Gtk::Notebook,Gtk::Paned,Gtk::Pixmap,Gtk::RadioButton,Gtk::Range,Gtk::ScrolledWindow,Gtk::Separator,Gtk::Table,Gtk::Toolbar,Gtk::AspectFrame,Gtk::Frame,Gtk::VBox,Gtk::HBox,Gtk::VSeparator,Gtk::HSeparator`

