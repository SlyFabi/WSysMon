#include "PerformanceButton.h"
#include "../../utils/Utils.h"

PerformanceButton::PerformanceButton(int viewId)
    : Glib::ObjectBase("PerformanceButton"), Gtk::Widget() {
    m_ViewId = viewId;
    m_Graph = new GraphWidget(0, 100, 60);
    m_Graph->SetDrawAxisText(false);

    set_has_window(true);
}

PerformanceButton::~PerformanceButton() {
    SAFE_DELETE(m_Graph);
}

void PerformanceButton::AddGraphPoint(double val) {
    m_Graph->AddPoint(val);
    this->queue_draw();
}

void PerformanceButton::AddGraphPoint2(double val) {
    m_Graph->AddPoint2(val);
    this->queue_draw();
}

GraphWidget *PerformanceButton::GetGraph() {
    return m_Graph;
}

int PerformanceButton::GetViewId() const {
    return m_ViewId;
}

sigc::signal<void(PerformanceButton *)> PerformanceButton::signal_click() {
    return m_SignalClick;
}

void PerformanceButton::SetTitle(const std::string& title) {
    m_Title = title;
}

void PerformanceButton::SetInfoText(const std::string& infoText) {
    m_InfoText = infoText;
}

void PerformanceButton::SetInfoText2(const std::string& infoText) {
    m_InfoText2 = infoText;
}

void PerformanceButton::set_selected(bool isSelected) {
    m_IsSelected = isSelected;
    this->queue_draw();

    if(isSelected) {
        for(auto button : m_OtherButtons) {
            if(button != this)
                button->set_selected(false);
        }
    }
}

void PerformanceButton::set_other_buttons(const std::vector<PerformanceButton *>& buttons) {
    m_OtherButtons = buttons;
}

bool PerformanceButton::on_button_press_event(GdkEventButton *button_event) {
    if(button_event->type == GdkEventType::GDK_BUTTON_PRESS && button_event->button == 1) {
        set_selected(true);
        m_SignalClick.emit(this);
    }

    return true;
}

Gtk::SizeRequestMode PerformanceButton::get_request_mode_vfunc() const {
    return Widget::get_request_mode_vfunc();
}

void PerformanceButton::get_preferred_width_vfunc(int &minimum_width, int &natural_width) const {
    minimum_width = 280;
    natural_width = 280;
}

void
PerformanceButton::get_preferred_height_for_width_vfunc(int width, int &minimum_height, int &natural_height) const {
    minimum_height = 110;
    natural_height = 110;
}

void PerformanceButton::get_preferred_height_vfunc(int &minimum_height, int &natural_height) const {
    minimum_height = 110;
    natural_height = 110;
}

void PerformanceButton::get_preferred_width_for_height_vfunc(int height, int &minimum_width, int &natural_width) const {
    minimum_width = 280;
    natural_width = 280;
}

void PerformanceButton::on_size_allocate(Gtk::Allocation &allocation) {
    set_allocation(allocation);

    if(m_refGdkWindow) {
        m_refGdkWindow->move_resize(allocation.get_x(), allocation.get_y(),
                                     allocation.get_width(), allocation.get_height());
    }
}

void PerformanceButton::on_map() {
    Widget::on_map();
}

void PerformanceButton::on_unmap() {
    Widget::on_unmap();
}

void PerformanceButton::on_realize() {
    set_realized();

    if(!m_refGdkWindow) {
        //Create the GdkWindow:
        GdkWindowAttr attributes;
        memset(&attributes, 0, sizeof(attributes));

        Gtk::Allocation allocation = get_allocation();

        //Set initial position and size of the Gdk::Window:
        attributes.x = allocation.get_x();
        attributes.y = allocation.get_y();
        attributes.width = allocation.get_width();
        attributes.height = allocation.get_height();

        attributes.event_mask = get_events () | Gdk::EXPOSURE_MASK;
        attributes.window_type = GDK_WINDOW_CHILD;
        attributes.wclass = GDK_INPUT_OUTPUT;

        m_refGdkWindow = Gdk::Window::create(get_parent_window(), &attributes,
                                             GDK_WA_X | GDK_WA_Y);
        set_window(m_refGdkWindow);

        //set colors
        override_background_color(Gdk::RGBA("red"));
        override_color(Gdk::RGBA("blue"));

        //make the widget receive expose events
        m_refGdkWindow->set_user_data(gobj());

        m_refGdkWindow->set_events(Gdk::EXPOSURE_MASK | Gdk::BUTTON_PRESS_MASK
        | Gdk::KEY_PRESS_MASK |
        Gdk::ENTER_NOTIFY_MASK );

        auto cursor = Gdk::Cursor::create(Gdk::HAND1);
        m_refGdkWindow->set_cursor(cursor);
    }
}

void PerformanceButton::on_unrealize() {
    m_refGdkWindow.reset();
    Gtk::Widget::on_unrealize();
}

bool PerformanceButton::on_draw(const Cairo::RefPtr<Cairo::Context> &cr) {
    // paint the background
    Gdk::RGBA background;
    background.set_rgba(0., 0., 0., 0.);
    Gdk::RGBA backgroundSelected;
    backgroundSelected.set_rgba(0.54296875, 0.78515625, 1., 0.4);

    Gdk::Cairo::set_source_rgba(cr, m_IsSelected ? backgroundSelected : background);
    cr->paint();

    // draw the foreground
    GdkRectangle graphRect;
    graphRect.x = 15;
    graphRect.y = 15;
    graphRect.width = 110;
    graphRect.height = 80;
    m_Graph->Draw(cr, graphRect);

    Gdk::RGBA textColor;
    textColor.set_rgba(0., 0., 0., 1.);
    if(get_style_context())
        get_style_context()->lookup_color("theme_text_color", textColor);
    Gdk::Cairo::set_source_rgba(cr, textColor);

    cr->move_to(140, 30);
    cr->set_font_size(22);
    cr->show_text(m_Title);

    cr->move_to(140, 50);
    cr->set_font_size(15);
    cr->show_text(m_InfoText);

    cr->move_to(140, 70);
    cr->show_text(m_InfoText2);

    return true;
}
