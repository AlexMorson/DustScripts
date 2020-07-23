#include '../fonts.cpp';

class Style
{
	
	uint text_clr						= 0xffffffff;
	uint normal_bg_clr					= 0xd9000000;
	uint normal_border_clr				= 0x33ffffff;
	uint highlight_bg_clr				= 0xd9622a24;
	uint highlight_border_clr			= 0xd9933c34;
	uint selected_bg_clr				= 0xd9000000;
	uint selected_border_clr			= 0xffb16860;
	uint selected_highlight_bg_clr		= 0xd9622a24;
	uint selected_highlight_border_clr	= 0xffb16860;
	uint disabled_bg_clr				= 0xa6000000;
	uint disabled_border_clr			= 0x26ffffff;
	uint shadow_clr						= 0x40000000;
	
	float disabled_alpha				= 0.35;
	
	float border_size = 1;
	float spacing = 4;
	float button_pressed_icon_offset = 1;
	
	// The default font used when creating labels, etc.
	string default_font = font::PROXIMANOVA_REG;
	// The default text size used when creating labels, etc.
	uint default_text_size = 26;
	// The default scaling for text - should be set before creating any UI. Changing it after may not reflect correctly everywhere.
	float default_text_scale = 0.75;
	
	int tooltip_fade_frames = 25;
	float tooltip_fade_offset_x = 0;
	float tooltip_fade_offset_y = 5;
	
	// Text measurements don't seem to line up exactly always. Use these global values to offset
	float text_offset_x = -1;
	float text_offset_y = -1;
	
	private string current_font = default_font;
	private uint current_text_size = default_text_size;
	private TextAlign current_align_v = TextAlign::Left;
	private TextAlign current_align_h = TextAlign::Top;
	private uint current_text_colour = text_clr;
	
	private textfield@ text_field;
	private dictionary sprite_sets;
	
	private float global_alpha = 1;
	private int global_alpha_size = 64;
	private array<float> global_alpha_stack(global_alpha_size);
	private int global_alpha_index;
	
	private scene@ g;
	
	uint _layer;
	uint _sub_layer;
	bool _hud;
	
	Style(bool hud)
	{
		_hud = hud;
		
		@g = get_scene();
		
		@text_field = create_textfield();
		text_field.set_font(current_font, current_text_size);
		text_field.align_horizontal(current_align_h);
		text_field.align_vertical(current_align_v);
	}
	
	void measure_text(const string text, const string font, const uint size, const float scale, float &out width, float &out height)
	{
		if(current_font != font || current_text_size != size)
			text_field.set_font(current_font = font, current_text_size = size);
		
		text_field.text(text);
		width  = text_field.text_width() * scale;
		height = text_field.text_height() * scale;
	}
	
	sprites@ get_sprite_for_set(const string &in sprite_set)
	{
		if(sprite_sets.exists(sprite_set))
		{
			return cast<sprites@>(sprite_sets[sprite_set]);
		}
		
		sprites@ sprite = create_sprites();
		sprite.add_sprite_set(sprite_set);
		@sprite_sets[sprite_set] = sprite;
		
		return sprite;
	}
	
	void _reset_state()
	{
		global_alpha = 1;
		global_alpha_index = 0;
	}
	
	void disable_alpha()
	{
		if(global_alpha_index == global_alpha_size)
		{
			global_alpha_size += 16;
			global_alpha_stack.resize(global_alpha_size);
		}
		
		global_alpha_stack[global_alpha_index++] = global_alpha;
		global_alpha *= clamp01(disabled_alpha);
	}
	
	void multiply_alpha(float alpha)
	{
		if(global_alpha_index == global_alpha_size)
		{
			global_alpha_size += 16;
			global_alpha_stack.resize(global_alpha_size);
		}
		
		global_alpha_stack[global_alpha_index++] = global_alpha;
		global_alpha *= clamp01(alpha);
	}
	
	void restore_alpha()
	{
		if(global_alpha_index == 0)
		{
			puts('ERROR: restore_alpha called more times than multiply_alpha');
			return;
		}
		
		global_alpha = global_alpha_stack[--global_alpha_index];
	}
	
	/*
	 * Basic drawing methods
	 */
	//{
		
		void draw_rectangle(
			const float x1, const float y1, const float x2, const float y2,
			const float rotation, uint colour) const
		{
			if(global_alpha != 1)
				colour = set_alpha(colour);
			
			if(_hud)
				g.draw_rectangle_hud(_layer, _sub_layer, x1, y1, x2, y2, rotation, colour);
			else
				g.draw_rectangle_world(_layer, _sub_layer, x1, y1, x2, y2, rotation, colour);
		}
		
		void draw_glass(
			const float x1, const float y1, const float x2, const float y2,
			const float rotation, uint colour) const
		{
			if(global_alpha != 1)
				colour = set_alpha(colour);
			
			if(_hud)
				g.draw_glass_world(_layer, _sub_layer, x1, y1, x2, y2, rotation, colour);
		}

		void draw_gradient(
			const float x1, const float y1, const float x2, const float y2,
			uint c00, uint c10, uint c11, uint c01) const
		{
			if(global_alpha != 1)
			{
				c00 = set_alpha(c00);
				c10 = set_alpha(c10);
				c11 = set_alpha(c11);
				c01 = set_alpha(c01);
			}
			
			if(_hud)
				g.draw_gradient_hud(_layer, _sub_layer, x1, y1, x2, y2, c00, c10, c11, c01);
			else
				g.draw_gradient_world(_layer, _sub_layer, x1, y1, x2, y2, c00, c10, c11, c01);
		}

		void draw_line(
			const float x1, const float y1, const float x2, const float y2,
			const float width, uint colour) const
		{
			if(global_alpha != 1)
				colour = set_alpha(colour);
			
			const float dx = x2 - x1;
			const float dy = y2 - y1;
			const float length = sqrt(dx * dx + dy * dy);
			
			const float mx = (x1 + x2) * 0.5;
			const float my = (y1 + y2) * 0.5;
			
			if(_hud)
				g.draw_rectangle_hud(_layer, _sub_layer,
					mx - width, my - length * 0.5,
					mx + width, my + length * 0.5, atan2(-dx, dy) * RAD2DEG, colour);
			else
				g.draw_rectangle_world(_layer, _sub_layer,
					mx - width, my - length * 0.5,
					mx + width, my + length * 0.5, atan2(-dx, dy) * RAD2DEG, colour);
		}

		void draw_quad(
			const bool is_glass,
			const float x1, const float y1, const float x2, const float y2,
			const float x3, const float y3, const float x4, const float y4,
			uint c1, uint c2, uint c3, uint c4) const
		{
			if(global_alpha != 1)
			{
				c1 = set_alpha(c1);
				c2 = set_alpha(c2);
				c3 = set_alpha(c3);
				c4 = set_alpha(c4);
			}
			
			if(_hud)
				g.draw_quad_hud(_layer, _sub_layer, is_glass, x1, y1, x2, y2, x3, y3, x4, y4, c1, c2, c3, c4);
			else
				g.draw_quad_world(_layer, _sub_layer, is_glass, x1, y1, x2, y2, x3, y3, x4, y4, c1, c2, c3, c4);
		}
		
		void draw_sprite(
			sprites@ sprite,
			const string sprite_name, const uint frame, const uint palette,
			const float x, const float y, const float rotation,
			const float scale_x, const float scale_y,
			uint colour) const
		{
			if(global_alpha != 1)
				colour = set_alpha(colour);
			
			if(_hud)
				sprite.draw_world(_layer, _sub_layer, sprite_name, frame, palette, x, y, rotation, scale_x, scale_y, colour);
			else
				sprite.draw_world(_layer, _sub_layer, sprite_name, frame, palette, x, y, rotation, scale_x, scale_y, colour);
		}
		
		void draw_text(
			const string text,
			const float x, const float y,
			const float scale_x, const float scale_y,
			const float rotation, uint colour)
		{
			if(global_alpha != 1)
				colour = set_alpha(colour);
			
			if(current_text_colour != colour)
			{
				current_text_colour = colour;
				text_field.colour(colour);
			}
			
			text_field.text(text);
			
			if(_hud)
				text_field.draw_hud(_layer, _sub_layer, x, y, scale_x, scale_y, rotation);
			else
				text_field.draw_world(_layer, _sub_layer, x, y, scale_x, scale_y, rotation);
		}
		
		uint set_alpha(uint colour)
		{
			if(global_alpha == 0)
				return colour & 0x00ffffff;
			
			return (colour & 0x00ffffff) | (uint(global_alpha * ((colour >> 24) & 0xff)) << 24);
		}
		
	//}
	
	/*
	 * Drawing methods
	 */
	//{
		
		void draw_interactive_element(const Element@ &in element, const bool &in highlighted, const bool &in selected, const bool &in disabled)
		{
			// Fill/bg
			
			const uint bg_clr = disabled ? disabled_bg_clr
				: (highlighted && selected ? selected_highlight_bg_clr
					: selected ? selected_bg_clr : (highlighted ? highlight_bg_clr : normal_bg_clr));
			
			draw_rectangle(
				element.x1, element.y1, element.x2, element.y2,
				0, bg_clr);
			
			// Border
			
			const uint border_clr = disabled ? disabled_border_clr
				: (highlighted && selected ? selected_highlight_border_clr
					: selected ? selected_border_clr : (highlighted ? highlight_border_clr : normal_border_clr));
			
			outline(element.x1, element.y1, element.x2, element.y2, border_size, border_clr);
		}
		
		void draw_text(const string text, const float x, const float y, const TextAlign align_h, const TextAlign align_v, const uint colour, float scale=-1, string font='', uint size=0)
		{
			if(scale <= 0)
			{
				scale = default_text_scale;
			}
			
			if(font == '')
			{
				font = default_font;
			}
			
			if(size == 0)
			{
				size = default_text_size;
			}
			
			if(current_font != font || current_text_size != size)
				text_field.set_font(current_font = font, current_text_size = size);
			
			if(current_align_h != align_h)
				text_field.align_horizontal(current_align_h = align_h);
			
			if(current_align_v != align_v)
				text_field.align_vertical(current_align_v = align_v);
			
			draw_text(
				text,
				x + text_offset_x * scale, y + text_offset_y * scale,
				scale, scale, 0, colour);
		}
		
		/**
		 * @brief Outlines the given rect. The outline is on the inside if thickness is positive, and on the outside if negative.
		 */
		void outline(
			const float x1, const float y1, const float x2, const float y2,
			const float thickness, const uint colour) const
		{
			//Left
			draw_rectangle(
				x1,
				y1 + thickness,
				x1 + thickness,
				y2 - thickness, 0, colour);
			// Right
			draw_rectangle(
				x2 - thickness,
				y1 + thickness,
				x2,
				y2 - thickness, 0, colour);
			// Top
			draw_rectangle(
				x1,
				y1,
				x2,
				y1 + thickness, 0, colour);
			// Bottom
			draw_rectangle(
				x1,
				y2 - thickness,
				x2,
				y2, 0, colour);
		}
	
	//}
	
}