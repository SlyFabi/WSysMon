#ifndef WSYSMON_GRAPHWIDGET_H
#define WSYSMON_GRAPHWIDGET_H

#include <gtkmm.h>

struct GraphPoint {
    double val;
    double timeSeconds;
};

struct GraphCoords {
    double x;
    double y;
};

class GraphWidget : public Gtk::Widget {
public:
    explicit GraphWidget(double minValue, double maxValue, double timespanSeconds);

    void AddPoint(double val);
    void AddPoint2(double val);

    void SetAxisColor(const Gdk::RGBA& color);
    void SetSecondaryColor(const Gdk::RGBA& color);

    void SetMargin(int marginH, int marginV);
    void SetDrawAxisText(bool draw);

    void SetSize(int width, int height);
    void SetLimits(double minValue, double maxValue);
    void SetAxisUnit(const std::string& unitStr);

    void Draw(const Cairo::RefPtr<Cairo::Context>& cr, GdkRectangle root);

protected:
    Gtk::SizeRequestMode get_request_mode_vfunc() const override;
    void get_preferred_width_vfunc(int& minimum_width, int& natural_width) const override;
    void get_preferred_height_for_width_vfunc(int width, int& minimum_height, int& natural_height) const override;
    void get_preferred_height_vfunc(int& minimum_height, int& natural_height) const override;
    void get_preferred_width_for_height_vfunc(int height, int& minimum_width, int& natural_width) const override;
    void on_size_allocate(Gtk::Allocation& allocation) override;
    void on_map() override;
    void on_unmap() override;
    void on_realize() override;
    void on_unrealize() override;
    bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;

    Glib::RefPtr<Gdk::Window> m_refGdkWindow;
    Glib::RefPtr<Gtk::CssProvider> m_refStyleProvider;

private:
    void DrawGraph(const Cairo::RefPtr<Cairo::Context> &cr, GdkRectangle plotArea, std::vector<GraphPoint>& points, Gdk::RGBA& color);
    GraphCoords PointToCoords(GdkRectangle plotArea, double x, double y) const;
    static bool CoordsValid(GdkRectangle plotArea, GraphCoords coords);

    int m_Width{};
    int m_Height{};

    Gdk::RGBA m_AxisColor;
    Gdk::RGBA m_SecondaryColor;

    double m_MinValue;
    double m_MaxValue;
    double m_TimespanSeconds;

    int m_MarginH, m_MarginV;
    bool m_DrawAxisText{};
    std::string m_AxisUnit;

    std::vector<GraphPoint> m_Points;
    std::vector<GraphPoint> m_Points2;
};

#endif //WSYSMON_GRAPHWIDGET_H
