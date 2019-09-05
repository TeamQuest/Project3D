#include <Urho3D/Core/Context.h>

template <typename Component, typename... Args>
void register_component(Urho3D::Context* context, Args&&... args)
{
    context->RegisterFactory<Component>(std::forward<Args>(args)...);
}

#define FACTORY_METHOD(f_name, f_invoke)              \
    template <typename... Args>                       \
    FactoryProxy& f_name(Args&&... args)              \
    {                                                 \
        value->f_invoke(std::forward<Args>(args)...); \
        return *this;                                 \
    }

template <typename T>
class FactoryProxy {
public:
    template <typename U, typename... Args>
    friend FactoryProxy<U> make(Args&&...);

    FactoryProxy(const FactoryProxy&) = delete;
    FactoryProxy& operator=(const FactoryProxy&) = delete;

private:
    FactoryProxy(T* value) : value{value}
    {
    }

public:
    FACTORY_METHOD(name, SetName);
    FACTORY_METHOD(style, SetStyle);
    FACTORY_METHOD(size, SetSize);
    FACTORY_METHOD(aligned, SetAlignment);
    FACTORY_METHOD(alignment, SetAlignment);
    FACTORY_METHOD(styleauto, SetStyleAuto);
    FACTORY_METHOD(layout, SetLayout);
    FACTORY_METHOD(position, SetPosition);
    FACTORY_METHOD(text, SetText);
    FACTORY_METHOD(textaligned, SetTextAlignment);
    FACTORY_METHOD(textalignment, SetTextAlignment);
    FACTORY_METHOD(font, SetFont);
    FACTORY_METHOD(fontsize, SetFontSize);
    FACTORY_METHOD(width, SetWidth)
    FACTORY_METHOD(height, SetHeight);
    FACTORY_METHOD(minsize, SetMinSize);
    FACTORY_METHOD(minwidth, SetMinWidth);
    FACTORY_METHOD(minheight, SetMinHeight);
    FACTORY_METHOD(maxsize, SetMaxSize);
    FACTORY_METHOD(maxwidth, SetMaxWidth);
    FACTORY_METHOD(maxheight, SetMaxHeight);
    FACTORY_METHOD(fixedsize, SetFixedSize);
    FACTORY_METHOD(fixedwidth, SetFixedWidth);
    FACTORY_METHOD(fixedheight, SetFixedHeight);
    FACTORY_METHOD(horizontalalignment, SetHorizontalAlignment);
    FACTORY_METHOD(verticalalignment, SetVerticalAlignment);
    FACTORY_METHOD(enableanchor, SetEnableAnchor);
    FACTORY_METHOD(minanchor, SetMinAnchor);
    FACTORY_METHOD(maxanchor, SetMaxAnchor);
    FACTORY_METHOD(minoffset, SetMinOffset);
    FACTORY_METHOD(maxoffset, SetMaxOffset);
    FACTORY_METHOD(pivot, SetPivot);
    FACTORY_METHOD(clipborder, SetClipBorder);
    FACTORY_METHOD(color, SetColor);
    FACTORY_METHOD(priority, SetPriority);
    FACTORY_METHOD(opacity, SetOpacity);
    FACTORY_METHOD(bringtofront, SetBringToFront);
    FACTORY_METHOD(bringtoback, SetBringToBack);
    FACTORY_METHOD(clipchildren, SetClipChildren);
    FACTORY_METHOD(sortchildren, SetSortChildren);
    FACTORY_METHOD(usederivedopacity, SetUseDerivedOpacity);
    FACTORY_METHOD(enabled, SetEnabled);
    FACTORY_METHOD(deepenabled, SetDeepEnabled);
    FACTORY_METHOD(enabledrecursive, SetEnabledRecursive);
    FACTORY_METHOD(editable, SetEditable);
    FACTORY_METHOD(focus, SetFocus);
    FACTORY_METHOD(selected, SetSelected);
    FACTORY_METHOD(visible, SetVisible);
    FACTORY_METHOD(focusmode, SetFocusMode);
    FACTORY_METHOD(dragdropmode, SetDragDropMode);
    FACTORY_METHOD(defaultstyle, SetDefaultStyle);
    FACTORY_METHOD(layoutmode, SetLayoutMode);
    FACTORY_METHOD(layoutspacing, SetLayoutSpacing);
    FACTORY_METHOD(layoutborder, SetLayoutBorder);
    FACTORY_METHOD(layoutflexscale, SetLayoutFlexScale);
    FACTORY_METHOD(indent, SetIndent);
    FACTORY_METHOD(indentspacing, SetIndentSpacing);
    FACTORY_METHOD(parent, SetParent);
    FACTORY_METHOD(var, SetVar);
    FACTORY_METHOD(internal, SetInternal);
    FACTORY_METHOD(traversalmode, SetTraversalMode);
    FACTORY_METHOD(elementeventsender, SetElementEventSender);
    FACTORY_METHOD(tags, SetTags);
    FACTORY_METHOD(texteffect, SetTextEffect);
    FACTORY_METHOD(effectstrokethickness, SetEffectStrokeThickness);
    FACTORY_METHOD(effectcolor, SetEffectColor);

    T* operator*() const
    {
        return value;
    }

private:
    T* value;
};

template <typename T, typename... Args>
auto make(Args&&... args) -> FactoryProxy<T>
{
    return {new T{std::forward<Args>(args)...}};
}

template <typename T>
T* get(const Urho3D::Variant& variant)
{
    return dynamic_cast<T*>(const_cast<Urho3D::Variant&>(variant).GetPtr());
}
