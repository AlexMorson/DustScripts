#include '../../../fonts.cpp';
#include '../../../enums/GlobalVirtualButton.cpp';
#include '../../../math/math.cpp';
#include '../../utils/GraphicAlign.cpp';
#include '../../events/Event.cpp';
#include '../Checkbox.cpp';
#include '../Label.cpp';
#include '../LockedContainer.cpp';
#include 'LayerSelectorSet.cpp';
#include 'LayerSelectorType.cpp';

class LayerSelector : LockedContainer
{
	
	Event layer_select;
	Event sub_layer_select;
	
	// Properties
	
	protected LayerSelectorType _type = LayerSelectorType(-1);
	/*DONE*/ protected bool _multi_select = true;
	/*DONE*/ protected bool _allow_deselect = true;
	/*DONE*/ protected bool _drag_select = true;
	/*DONE*/ protected float _align_v = GraphicAlign::Top;
	/*DONE*/ protected bool  _labels_first = true;
	/*DONE*/ protected float _label_spacing = NAN;
	/*DONE*/ protected float _layer_spacing = NAN;
	/*DONE*/ protected float _spacing = NAN;
	/*DONE*/ protected bool _toggle_on_press = true;
	/*DONE*/ protected int _select_layer_group_modifier = GlobalVirtualButton::Shift;
	
	/*DONE*/ protected bool _show_all_layers_toggle = true;
	/*DONE*/ protected bool _show_all_sub_layers_toggle = true;
	/*DONE*/ protected bool _toggle_all_top = true;
	
	/*DONE*/ protected string _font = font::ENVY_BOLD;
	/*DONE*/ protected uint _font_size = 20;
	
	//
	
	protected bool validate_layout;
	
	protected LayerSelectorSet@ layers;
	protected LayerSelectorSet@ sub_layers;
	protected bool has_layers;
	protected bool has_sub_layers;
	
	LayerSelector(UI@ ui, const LayerSelectorType type=LayerSelectorType::Layers)
	{
		super(ui);
		
		background_colour = ui.style.normal_bg_clr;
		background_blur = true;
		
		this.type = type;
		
		validate_layout = true;
	}
	
	string element_type { get const override { return 'LayerSelector'; } }
	
	// ///////////////////////////////////////////////////////////////////
	// Bisax properties
	// ///////////////////////////////////////////////////////////////////
	
	/// Whether the layer list, the sub layer list, or both are shown.
	LayerSelectorType type
	{
		get const { return _type; }
		set
		{
			if(_type == value)
				return;
			
			_type = value;
			
			has_layers = false;
			has_sub_layers = false;
			validate_layout = true;
			
			if(_type == LayerSelectorType::Layers || _type == LayerSelectorType::Both)
			{
				has_layers = true;
				initialise_layers_set();
				layers.visible = true;
			}
			else if(@layers != null)
			{
				layers.visible = false;
			}
			
			if(_type == LayerSelectorType::SubLayers || _type == LayerSelectorType::Both)
			{
				has_sub_layers = true;
				initialise_sub_layers_set();
				sub_layers.visible = true;
			}
			else if(@sub_layers != null)
			{
				sub_layers.visible = false;
			}
		}
	}
	
	/// Sets whether or not multiple layers can be selected at once
	/// If mutliple layers are selected and this is disabled, only the highest selected layer will remain selected.
	bool multi_select
	{
		get const { return _multi_select; }
		set
		{
			if(_multi_select == value)
				return;
			
			if(has_layers)
				layers.update_multi_select(_multi_select);
			if(has_sub_layers)
				sub_layers.update_multi_select(_multi_select);
		}
	}
	
	/// Prevents layers from being deselected
	bool allow_deselect
	{
		get const { return _allow_deselect; }
		set
		{
			if(_allow_deselect == value)
				return;
			
			if(has_layers)
				layers.allow_deselect = _allow_deselect;
			if(has_sub_layers)
				sub_layers.allow_deselect = _allow_deselect;
		}
	}
	
	/// If this and multi_select is set, multiple layers can be selected or deselected by clicking and dragging the mouse.
	bool drag_select
	{
		get const { return _drag_select; }
		set
		{
			if(_drag_select == value)
				return;
			
			if(has_layers)
				layers.drag_select = _drag_select;
			if(has_sub_layers)
				sub_layers.drag_select = _drag_select;
		}
	}
	
	/// Only relevant when both the layer and sublayer lists are shown.
	/// Controls how they are aligned vertically.
	float align_v
	{
		get const { return _align_v; }
		set
		{
			if(_align_v == value)
				return;
			
			_align_v = value;
			validate_layout = true;
		}
	}
	
	/// If true, layers will be toggled when pressed instead of when clicked
	bool toggle_on_press
	{
		get const { return _toggle_on_press; }
		set
		{
			if(_toggle_on_press == value)
				return;
			
			_toggle_on_press = value;
			
			if(has_layers)
				layers.update_toggle_on_press(_toggle_on_press);
			if(has_sub_layers)
				sub_layers.update_toggle_on_press(_toggle_on_press);
		}
	}
	
	/// Sets the GVB key that can be used to toggle entire layer groups.
	/// Set to -1 to disable.
	int select_layer_group_modifier
	{
		get const { return _select_layer_group_modifier; }
		set
		{
			if(_select_layer_group_modifier == value)
				return;
			
			_select_layer_group_modifier = value;
			
			if(has_layers)
				layers.select_layer_group_modifier = _select_layer_group_modifier;
			if(has_sub_layers)
				sub_layers.select_layer_group_modifier = _select_layer_group_modifier;
		}
	}
	
	/// Controls whether the labels or checkboxes are displayed first on the left side
	bool labels_first
	{
		get const { return _labels_first; }
		set
		{
			_labels_first = value;
			
			if(has_layers)
				layers.update_labels_first(_labels_first);
			if(has_sub_layers)
				sub_layers.update_labels_first(_labels_first);
		}
	}
	
	/// The spacing between labels and checkboxes
	float label_spacing
	{
		get const { return _label_spacing; }
		set
		{
			if(_label_spacing == value)
				return;
			
			_label_spacing = value;
			
			if(has_layers)
				layers.update_label_spacing(_label_spacing);
			if(has_sub_layers)
				sub_layers.update_label_spacing(_label_spacing);
		}
	}
	
	/// The spacing between labels and checkboxes
	float layer_spacing
	{
		get const { return _layer_spacing; }
		set
		{
			if(_layer_spacing == value)
				return;
			
			_layer_spacing = value;
			
			if(has_layers)
				layers.update_layer_spacing(_layer_spacing);
			if(has_sub_layers)
				sub_layers.update_layer_spacing(_layer_spacing);
		}
	}
	
	/// The spacing between layers and sublayers
	float spacing
	{
		get const { return _spacing; }
		set
		{
			if(_spacing == value)
				return;
			
			_spacing = value;
			validate_layout = true;
		}
	}
	
	/// Whether not to show the toggle all layers checkbox
	bool show_all_layers_toggle
	{
		get const { return _show_all_layers_toggle; }
		set
		{
			if(_show_all_layers_toggle == value)
				return;
			
			_show_all_layers_toggle = value;
			
			if(has_layers && layers.update_toggle_all_visibility(_show_all_layers_toggle))
			{
				rebuild_layers();
			}
		}
	}
	
	/// Whether not to show the toggle all sublayers checkbox
	bool show_all_sub_layers_toggle
	{
		get const { return _show_all_sub_layers_toggle; }
		set
		{
			if(_show_all_sub_layers_toggle == value)
				return;
			
			_show_all_sub_layers_toggle = value;
			
			if(has_sub_layers && sub_layers.update_toggle_all_visibility(_show_all_sub_layers_toggle))
			{
				rebuild_sub_layers();
			}
		}
	}
	
	/// The toggle all checkboxes can be displayed at the bottom or the top
	bool toggle_all_top
	{
		get const { return _toggle_all_top; }
		set
		{
			if(_toggle_all_top == value)
				return;
			
			_toggle_all_top = value;
			
			if(has_layers)
				layers.toggle_all_top = _toggle_all_top;
			if(has_sub_layers)
				sub_layers.toggle_all_top = _toggle_all_top;
			
			invalidate();
		}
	}
	
	/// Sets the font used for all layer labels
	void set_font(const string font, const uint size)
	{
		if(_font == font && _font_size == size)
			return;
		
		_font = font;
		_font_size = size;
		
		if(has_layers)
			layers.set_font(font, size);
		if(has_sub_layers)
			sub_layers.set_font(font, size);
		
		invalidate();
	}
	
	// ///////////////////////////////////////////////////////////////////
	// Colours
	// ///////////////////////////////////////////////////////////////////
	
	/// Returns true and sets colour if the specified layer has custom colour set
	bool get_layer_colour(const int layer, uint &out colour)
	{
		return layers.get_layer_colour(layer, colour);
	}
	
	/// Sets a custom colour for the specified layer. If end_layer is not -1 will set all layers between layer and end_layer (inclusive)
	/// Returns true if the layer colours was succesfully set.
	bool set_layer_colour(const int layer, const uint colour, const int end_layer=-1)
	{
		return layers.set_layer_colour(layer, colour, end_layer);
	}
	
	/// Clears the custom colour for the specified layer if it has any. If end_layer is not -1 will set all layers between layer and end_layer (inclusive)
	/// Returns true if the layer colours was succesfully cleared.
	bool clear_layer_colour(const int layer, const int end_layer=-1)
	{
		return layers.clear_layer_colour(layer, end_layer);
	}
	
	/// Returns true and sets colour if the specified sub layer has custom colour set
	bool get_sub_layer_colour(const int sub_layer, uint &out colour)
	{
		return layers.get_layer_colour(sub_layer, colour);
	}
	
	/// Sets a custom colour for the specified sub layer. If end_sub_layer is not -1 will set all layers between sub_layer and end_sub_layer (inclusive)
	/// Returns true if the sub layer colours was succesfully set.
	bool set_sub_layer_colour(const int sub_layer, const uint colour, const int end_sub_layer=-1)
	{
		return layers.set_layer_colour(sub_layer, colour, end_sub_layer);
	}
	
	/// Clears the custom colour for the specified sub layer if it has any. If end_sub_layer is not -1 will set all layers between sub_layer and end_sub_layer (inclusive)
	/// Returns true if the sub layer colours was succesfully cleared.
	bool clear_sub_layer_colour(const int sub_layer, const int end_sub_layer=-1)
	{
		return layers.clear_layer_colour(sub_layer, end_sub_layer);
	}
	
	// ///////////////////////////////////////////////////////////////////
	// Layers
	// ///////////////////////////////////////////////////////////////////
	
	// Get
	
	/// Returns true if the given layer is visible
	bool get_layer_visibility(const int layer)
	{
		return layers.get_visible_count(layer, layer) > 0;
	}
	
	/// Returns true if the given sub layer is visible
	bool get_sub_layer_visibility(const int layer)
	{
		return sub_layers.get_visible_count(layer, layer) > 0;
	}
	
	/// Returns the number of visible layers in the given range
	int get_layer_visibility(const int start_layer, const int end_layer)
	{
		return layers.get_visible_count(start_layer, end_layer);
	}
	
	/// Returns the number of visible sublayers in the given range
	int get_sub_layer_visibility(const int start_layer, const int end_layer)
	{
		return sub_layers.get_visible_count(start_layer, end_layer);
	}
	
	// Set
	
	/// Sets the visibility of the given layer. Returns true if anything changed.
	bool set_layer_visibility(const int layer, const bool visible)
	{
		return update_layers_visiblity(layers, layer, layer, visible) != 0;
	}
	
	/// Sets the visibility of the given layer. Returns true if anything changed.
	bool set_sub_layer_visibility(const int sub_layer, const bool visible)
	{
		return update_layers_visiblity(sub_layers, sub_layer, sub_layer, visible) != 0;
	}
	
	/// Sets the visibility of the layers between start_layer and end_layer inclusive.
	/// Returns the number of changed layers.
	int set_layer_visibility(const int start_layer, const int end_layer, const bool visible)
	{
		return update_layers_visiblity(layers, start_layer, end_layer, visible);
	}
	
	/// Sets the visibility of the sublayers between start_layer and end_layer inclusive.
	/// Returns the number of changed layers.
	int set_sub_layer_visibility(const int start_layer, const int end_layer, const bool visible)
	{
		return update_layers_visiblity(sub_layers, start_layer, end_layer, visible);
	}
	
	// Groups get
	
 	int get_backdrop_layers_visibility()	{ return layers.get_visible_count(0, 5 ); }
	int get_parallax_layers_visibility()	{ return layers.get_visible_count(6, 11); }
	int get_background_layers_visibility()	{ return layers.get_visible_count(12, 17); }
	int get_entities_layer_visibility()		{ return layers.get_visible_count(18, 18); }
	int get_collision_layer_visibility()	{ return layers.get_visible_count(19, 19); }
	int get_foreground_layer_visibility()	{ return layers.get_visible_count(20, 20); }
	int get_ui_layers_visibility()			{ return layers.get_visible_count(21, 22); }
	
	// Groups set
	
 	bool set_backdrop_layers_visibility(const bool visible)		{ return update_layers_visiblity(layers, 0, 5,   visible) > 0; }
	bool set_parallax_layers_visibility(const bool visible)		{ return update_layers_visiblity(layers, 6, 11,  visible) > 0; }
	bool set_background_layers_visibility(const bool visible)	{ return update_layers_visiblity(layers, 12, 17, visible) > 0; }
	bool set_entities_layer_visibility(const bool visible)		{ return update_layers_visiblity(layers, 18, 18, visible) > 0; }
	bool set_collision_layer_visibility(const bool visible)		{ return update_layers_visiblity(layers, 19, 19, visible) > 0; }
	bool set_foreground_layer_visibility(const bool visible)	{ return update_layers_visiblity(layers, 20, 20, visible) > 0; }
	bool set_ui_layers_visibility(const bool visible)			{ return update_layers_visiblity(layers, 21, 22, visible) > 0; }
	
	// ///////////////////////////////////////////////////////////////////
	// Selection
	// ///////////////////////////////////////////////////////////////////
	
	/// Deselects all layers and returns the number that were changed
	int select_layers_none()
	{
		if(!has_layers)
			return 0;
		
		return layers.select_all();
	}
	
	/// Selects all layers and returns the number that were changed
	int select_layers_all()
	{
		if(!_multi_select || !has_layers)
			return 0;
		
		return layers.select_all();
	}
	
	/// Deselects all sub layers and returns the number that were changed
	int select_sub_layers_none()
	{
		if(!has_sub_layers)
			return 0;
		
		return sub_layers.select_all();
	}
	
	/// Selects all sub layers and returns the number that were changed
	int select_sub_layers_all()
	{
		if(!_multi_select || !has_sub_layers)
			return 0;
		
		return sub_layers.select_all();
	}
	
	// Layer groups set
	
	bool set_backdrop_layers_selected(const bool selected)		{ return layers.initialise_states(0, 5,   selected) > 0; }
	bool set_parallax_layers_selected(const bool selected)		{ return layers.initialise_states(6, 11,  selected) > 0; }
	bool set_background_layers_selected(const bool selected)	{ return layers.initialise_states(12, 17, selected) > 0; }
	bool set_entities_layer_selected(const bool selected)		{ return layers.initialise_states(18, 18, selected) > 0; }
	bool set_collision_layer_selected(const bool selected)		{ return layers.initialise_states(19, 19, selected) > 0; }
	bool set_foreground_layer_selected(const bool selected)		{ return layers.initialise_states(20, 20, selected) > 0; }
	bool set_ui_layers_selected(const bool selected)			{ return layers.initialise_states(21, 22, selected) > 0; }
	
	// Layer groups get
	
	int num_backdrop_layers_selected()		{ return layers.count_selected(0, 5); }
	int num_parallax_layers_selected()		{ return layers.count_selected(6, 11); }
	int num_background_layers_selected()	{ return layers.count_selected(12, 17); }
	int num_entities_layer_selected()		{ return layers.count_selected(18, 18); }
	int num_collision_layer_selected()		{ return layers.count_selected(19, 19); }
	int num_foreground_layer_selected()		{ return layers.count_selected(20, 20); }
	int num_ui_layers_selected()			{ return layers.count_selected(21, 22); }
	
	// TODO: Get number of selected layers and sub layers
	// TODO: Get number of selected layers and sub layers in range
	// TODO: Is layer/sublayer selected
	// TODO: Get list of all selected [sub]layers
	
	// ///////////////////////////////////////////////////////////////////
	// Protected/Internal
	// ///////////////////////////////////////////////////////////////////
	
	protected int update_layers_visiblity(LayerSelectorSet@ layers, const int start_layer, const int end_layer, const bool visible)
	{
		if(@layers == null)
			return 0;
		
		const int result = layers.update_visibility(start_layer, end_layer, visible);
		
		if(result > 0)
		{
			if(@layers == @this.layers)
				rebuild_layers();
			else
				rebuild_sub_layers();
		}
		
		return result;
	}
	
	void _do_layout(LayoutContext@ ctx) override
	{
		if(validate_layout)
		{
			float width = 0;
			float height = 0;
			
			const float layer_spacing = is_nan(_layer_spacing) ? ui.style.spacing : _layer_spacing;
			const float spacing = is_nan(_spacing) ? ui.style.spacing : _spacing;
			
			if(has_layers)
			{
				if(layers.validate_layout)
				{
					layers.do_layout();
				}
				
				height = max(layers._height, height);
				layers.x = ui.style.spacing;
				width += layers._width;
			}
			
			if(has_sub_layers)
			{
				if(sub_layers.validate_layout)
				{
					sub_layers.do_layout();
				}
				
				height = max(sub_layers._height, height);
				sub_layers.x = ui.style.spacing + width + (has_layers ? spacing : 0);
				width += sub_layers._width;
			}
			
			if(has_layers)
			{
				layers.y = ui.style.spacing - layer_spacing * 0.5 + (height - layers._height) * align_v;;
			}
			
			if(has_sub_layers)
			{
				sub_layers.y = ui.style.spacing - layer_spacing * 0.5 + (height - sub_layers._height) * align_v;;
			}
			
			this.width = width + ui.style.spacing * 2 + (has_layers && has_sub_layers ? spacing : 0);
			this.height = height + ui.style.spacing * 2 - layer_spacing;
			
			validate_layout = false;
		}
	}
	
	protected void invalidate()
	{
		validate_layout = true;
		
		if(has_layers)
			layers.validate_layout = true;
		if(has_sub_layers)
			sub_layers.validate_layout = true;
	}
	
	protected void initialise_layers_set()
	{
		if(@layers != null)
			return;
		
		// 24 = 21 layers (0-20) + 2 ui layers + toggle all
		@layers = LayerSelectorSet(ui, this, 24, @layer_select, EventType::LAYER_SELECT);
		
		layers.initialise_layer_values(0, 5,   0, true);
		layers.initialise_layer_values(6, 11,  1, true);
		layers.initialise_layer_values(12, 17, 2, true);
		layers.initialise_layer_values(18, 18, 3, true);
		layers.initialise_layer_values(19, 19, 4, true);
		layers.initialise_layer_values(20, 20, 5, true);
		layers.initialise_layer_values(21, 22, 6, true);
		layers.initialise_layer_values(23, 23, 7, _show_all_layers_toggle && _multi_select);

		
		initialise_layers_set_generic(layers);
		rebuild_layers();
		
		Container::add_child(layers);
	}
	
	protected void initialise_sub_layers_set()
	{
		if(@sub_layers != null)
			return;
		
		// 26 = 25 sub layers (0-24) + toggle all
		@sub_layers = LayerSelectorSet(ui, this, 26, @sub_layer_select, EventType::SUB_LAYER_SELECT);
		
		sub_layers.initialise_layer_values(0, 24, 0, true);
		sub_layers.initialise_layer_values(25, 25, 1, _show_all_sub_layers_toggle && _multi_select);
		
		initialise_layers_set_generic(sub_layers);
		rebuild_sub_layers();
		
		Container::add_child(sub_layers);
	}
	
	protected void initialise_layers_set_generic(LayerSelectorSet@ layers)
	{
		layers.allow_deselect				= _allow_deselect;
		layers.multi_select					= _multi_select;
		layers.drag_select					= _drag_select;
		layers.toggle_on_press				= _toggle_on_press;
		layers.select_layer_group_modifier	= _select_layer_group_modifier;
		
		layers.labels_first		= _labels_first;
		layers.label_spacing	= _label_spacing;
		layers.layer_spacing	= _layer_spacing;
		
		layers.toggle_all_top	= _toggle_all_top;
		
		layers.font			= _font;
		layers.font_size	= _font_size;
	}
	
	protected void rebuild_layers()
	{
		layers.rebuild();
		
		layers.rebuild_checkboxes(true);
		
		layers.rebuild_complete();
		validate_layout = true;
	}
	
	protected void rebuild_sub_layers()
	{
		sub_layers.rebuild();
		
		sub_layers.rebuild_checkboxes(false);
		
		sub_layers.rebuild_complete();
		validate_layout = true;
	}
	
}