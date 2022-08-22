#include <spdlog/fmt/fmt.h>

#include "../../utils/Utils.h"
#include "GraphWidget.h"

GraphWidget::GraphWidget(double minValue, double maxValue, double timespanSeconds)
    : Glib::ObjectBase("GraphWidget"), Gtk::Widget() {
    m_MinValue = minValue;
    m_MaxValue = maxValue;
    m_TimespanSeconds = timespanSeconds;
    m_Margin = 0;
    m_DrawAxisText = true;

    m_AxisColor.set_rgba(74 / 256., 120 / 256., 149 / 256., 1);
    m_SecondaryColor.set_rgba(174 / 256., 120 / 256., 90 / 256., 1);
    m_AxisUnit = "% Usage";

    set_has_window(true);
    SetSize(120, 80);
}

void GraphWidget::AddPoint(double val) {
    if(val > m_MaxValue)
        m_MaxValue = val;

    auto timeMs = Utils::GetCurrentTimeMS();
    auto timeS = (double)timeMs / 1000.;

    GraphPoint point{};
    point.val = val;
    point.timeSeconds = timeS;

    m_Points.push_back(point);
    this->queue_draw();
}

void GraphWidget::AddPoint2(double val) {
    if(val > m_MaxValue)
        m_MaxValue = val;

    auto timeMs = Utils::GetCurrentTimeMS();
    auto timeS = (double)timeMs / 1000.;

    GraphPoint point{};
    point.val = val;
    point.timeSeconds = timeS;

    m_Points2.push_back(point);
    this->queue_draw();
}

void GraphWidget::SetAxisColor(const Gdk::RGBA& color) {
    m_AxisColor = color;
}

void GraphWidget::SetSecondaryColor(const Gdk::RGBA& color) {
    m_SecondaryColor = color;
}

void GraphWidget::SetMargin(int margin) {
    m_Margin = margin;
}

void GraphWidget::SetDrawAxisText(bool draw) {
    m_DrawAxisText = draw;
}

void GraphWidget::SetSize(int width, int height) {
    m_Width = width;
    m_Height = height;
}

void GraphWidget::SetLimits(double minValue, double maxValue) {
    m_MinValue = minValue;
    m_MaxValue = maxValue;
}

void GraphWidget::SetAxisUnit(const std::string &unitStr) {
    m_AxisUnit = unitStr;
}

Gtk::SizeRequestMode GraphWidget::get_request_mode_vfunc() const {
    return Widget::get_request_mode_vfunc();
}

void GraphWidget::get_preferred_width_vfunc(int &minimum_width, int &natural_width) const {
    minimum_width = m_Width;
    natural_width = m_Width;
}

void GraphWidget::get_preferred_height_for_width_vfunc(int width, int &minimum_height, int &natural_height) const {
    minimum_height = m_Height;
    natural_height = m_Height;
}

void GraphWidget::get_preferred_height_vfunc(int &minimum_height, int &natural_height) const {
    minimum_height = m_Height;
    natural_height = m_Height;
}

void GraphWidget::get_preferred_width_for_height_vfunc(int height, int &minimum_width, int &natural_width) const {
    minimum_width = m_Width;
    natural_width = m_Width;
}

void GraphWidget::on_size_allocate(Gtk::Allocation &allocation) {
    set_allocation(allocation);
    if(m_refGdkWindow) {
        m_refGdkWindow->move_resize(allocation.get_x(), allocation.get_y(),
                                    allocation.get_width(), allocation.get_height());
    }
}

void GraphWidget::on_map() {
    Widget::on_map();
}

void GraphWidget::on_unmap() {
    Widget::on_unmap();
}

void GraphWidget::on_realize() {
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

        //make the widget receive expose events
        m_refGdkWindow->set_user_data(gobj());

        m_refGdkWindow->set_events(Gdk::EXPOSURE_MASK | Gdk::BUTTON_PRESS_MASK
        | Gdk::KEY_PRESS_MASK |
        Gdk::ENTER_NOTIFY_MASK );
    }
}

void GraphWidget::on_unrealize() {
    m_refGdkWindow.reset();

    //Call base class:
    Gtk::Widget::on_unrealize();
}

bool GraphWidget::on_draw(const Cairo::RefPtr<Cairo::Context> &cr) {
    GdkRectangle root;
    m_refGdkWindow->get_geometry(root.x, root.y, root.width, root.height);
    root.x = 0;
    root.y = 0;
    Draw(cr, root);
    return true;
}

void GraphWidget::Draw(const Cairo::RefPtr<Cairo::Context> &cr, GdkRectangle root) {
    auto margin = m_Margin;
    GdkRectangle plotArea;
    plotArea.x = root.x + margin;
    plotArea.y = root.y + margin;
    plotArea.width = root.width - margin * 2;
    plotArea.height = root.height - margin * 2;

    // paint the background
    Gdk::RGBA background;
    background.set_rgba(0., 0., 0., 0.);
    Gdk::Cairo::set_source_rgba(cr, background);
    cr->paint();

    if(get_style_context())
        get_style_context()->lookup_color("theme_bg_color", background);

    Gdk::Cairo::set_source_rgba(cr, background);
    cr->rectangle(plotArea.x, plotArea.y, plotArea.width, plotArea.height);
    cr->fill();

    // Draw points
    DrawGraph(cr, plotArea, m_Points, m_AxisColor);
    DrawGraph(cr, plotArea, m_Points2, m_SecondaryColor);

    // Draw outline
    Gdk::Cairo::set_source_rgba(cr, m_AxisColor);

    cr->set_line_width(3);
    cr->move_to(plotArea.x, plotArea.y);
    cr->line_to(plotArea.x + plotArea.width, plotArea.y);
    cr->move_to(plotArea.x, plotArea.y);
    cr->line_to(plotArea.x, plotArea.y + plotArea.height);
    cr->move_to(plotArea.x + plotArea.width, plotArea.y);
    cr->line_to(plotArea.x + plotArea.width, plotArea.y + plotArea.height);
    cr->move_to(plotArea.x, plotArea.y + plotArea.height);
    cr->line_to(plotArea.x + plotArea.width, plotArea.y + plotArea.height);
    cr->stroke();

    // Graph texts
    if(m_DrawAxisText) {
        Gdk::RGBA textColor;
        textColor.set_rgba(0.6, 0.6, 0.6);
        Gdk::Cairo::set_source_rgba(cr, textColor);

        auto pangoLayout = Pango::Layout::create(cr);
        int textWidth;
        int textHeight;
        pangoLayout->get_pixel_size(textWidth, textHeight);

        auto textOffset = 25;
        pangoLayout->set_text(m_AxisUnit);
        pangoLayout->get_pixel_size(textWidth, textHeight);
        cr->move_to(plotArea.x, plotArea.y - textOffset / 2. - textHeight / 2.);
        pangoLayout->show_in_cairo_context(cr);

        pangoLayout->set_text(std::to_string((int)m_MaxValue));
        pangoLayout->get_pixel_size(textWidth, textHeight);
        cr->move_to(plotArea.x + plotArea.width - textWidth, plotArea.y - textOffset / 2. - textHeight / 2.);
        pangoLayout->show_in_cairo_context(cr);

        textOffset = 15;
        pangoLayout->set_text(fmt::format("{} Seconds", (int)m_TimespanSeconds));
        pangoLayout->get_pixel_size(textWidth, textHeight);
        cr->move_to(plotArea.x, plotArea.y + plotArea.height + textOffset - textHeight / 2.);
        pangoLayout->show_in_cairo_context(cr);

        pangoLayout->set_text(std::to_string((int)m_MinValue));
        pangoLayout->get_pixel_size(textWidth, textHeight);
        cr->move_to(plotArea.x + plotArea.width - textWidth, plotArea.y + plotArea.height + textOffset - textHeight / 2.);
        pangoLayout->show_in_cairo_context(cr);
    }
}

void GraphWidget::DrawGraph(const Cairo::RefPtr<Cairo::Context> &cr, GdkRectangle plotArea, std::vector<GraphPoint> &points, Gdk::RGBA& color) {
    Gdk::Cairo::set_source_rgba(cr, color);
    if(!points.empty()) {
        cr->set_line_width(2);

        auto firstPoint = points[points.size() - 1];
        auto startTime = firstPoint.timeSeconds;

        auto coords = PointToCoords(plotArea, 0, firstPoint.val);
        cr->move_to(coords.x, coords.y);

        int index = 0;
        std::vector<GraphPoint> toRemove;
        for (auto it = points.rbegin(); it != points.rend(); ++it) {
            auto time = startTime - it->timeSeconds;
            coords = PointToCoords(plotArea, time, it->val);
            if(CoordsValid(plotArea, coords))
                cr->line_to(coords.x, coords.y);
            else
                toRemove.push_back(*it);

            // First bottom line
            if(index == points.size() - 1) {
                if(time > m_TimespanSeconds)
                    time = m_TimespanSeconds;

                coords = PointToCoords(plotArea, time, 0);
                cr->line_to(coords.x, coords.y);

                coords = PointToCoords(plotArea, 0, 0);
                cr->line_to(coords.x, coords.y);
            }

            index++;
        }

        // Remove out of range
        for(auto it : toRemove)  {
            auto found = std::find_if(points.begin(), points.end(),[&](GraphPoint point) -> bool { return it.val == point.val && it.timeSeconds == point.timeSeconds; });
            if(found != std::end(points))
                points.erase(found);
        }
    }

    cr->stroke_preserve();
    Gdk::RGBA fillColor(color);
    fillColor.set_alpha(0.1);
    Gdk::Cairo::set_source_rgba(cr, fillColor);
    cr->fill();
}

GraphCoords GraphWidget::PointToCoords(GdkRectangle plotArea, double x, double y) const { // x = Time, y = val
    x = m_TimespanSeconds - x; // Invert time axis

    y -= m_MinValue;
    y = m_MaxValue - y; // Invert val axis

    auto widthScale = plotArea.width / m_TimespanSeconds;
    auto heightScale = plotArea.height / (m_MaxValue - m_MinValue);

    GraphCoords coords{};
    coords.x = plotArea.x + x * widthScale;
    coords.y = plotArea.y + y * heightScale;
    return coords;
}

bool GraphWidget::CoordsValid(GdkRectangle plotArea, GraphCoords coords) {
    return coords.x >= plotArea.x && coords.y >= plotArea.y && coords.x <= plotArea.x + plotArea.width && coords.y <= plotArea.y + plotArea.height;
}
