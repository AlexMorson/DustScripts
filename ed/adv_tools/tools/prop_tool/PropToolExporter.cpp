namespace PropToolExporter
{
	
	void sprite_batch(array<PropData@>@ data, const int num_props, const float origin_x, const float origin_y)
	{
		string sprite_set_name = '';
		string layer_sub_layer = '';
		string x_y = '';
		string scale = '';
		string rotation = '';
		string palette = '';
		
		for(int i = 0; i < num_props; i++)
		{
			prop@ p = data[i].prop;
			string sprite_set, sprite_name;
			sprite_from_prop(p, sprite_set, sprite_name);
			
			sprite_set_name += "'" + sprite_set + "','" + sprite_name + "',";
			layer_sub_layer += p.layer() + ',' + p.sub_layer() + ',';
			x_y += (p.x() - origin_x) + ',' + (p.y() - origin_y) + ',';
			scale += p.scale_x() + ',' + p.scale_y() + ',';
			rotation += p.rotation() + ',';
			palette += p.palette() + ',';
		}
		
		puts('========================================================================');
		puts('SpriteBatch spr(\n' +
			'\tarray<string>={' + sprite_set_name + '},\n'
			'\tarray<int>={' + layer_sub_layer + '},\n'
			'\tarray<float>={' + x_y + '},\n'
			'\tarray<float>={' + scale + '},\n'
			'\tarray<float>={' + rotation + '},\n'
			'\tarray<uint>={' + palette + '});');
		puts('========================================================================');
	}
	
	void sprite_group(array<PropData@>@ data, const int num_props, const float origin_x, const float origin_y)
	{
		string sprite_set_name = '';
		string layer_sub_layer = '';
		string align_offset_rotation_scale = '';
		string colour_frame_palette = '';
		sprites@ spr = create_sprites();
		
		for(int i = 0; i < num_props; i++)
		{
			prop@ p = data[i].prop;
			string sprite_set, sprite_name;
			sprite_from_prop(p, sprite_set, sprite_name);
			
			float scale_x = p.scale_x();
			float scale_y = p.scale_y();
			float rot = p.rotation();
			
			const float align_x = 0.5;
			const float align_y = 0.5;
			
			spr.add_sprite_set(sprite_set);
			rectangle@ rect = spr.get_sprite_rect(sprite_name, 0);
			float ox = (rect.left() + rect.get_width() * align_x) * scale_x;
			float oy = (rect.top() + rect.get_height() * align_y) * scale_y;
			rotate(ox, oy, rot * DEG2RAD, ox, oy);
			
			float off_x = p.x() - origin_x + ox;
			float off_y = p.y() - origin_y + oy;
			
			sprite_set_name += "'" + sprite_set + "','" + sprite_name + "',";
			layer_sub_layer += p.layer() + ',' + p.sub_layer() + ',';
			align_offset_rotation_scale += align_x + ',' + align_y + ',' + off_x + ',' + off_y + ',' + rot + ',' + scale_x + ',' + scale_y + ',';
			colour_frame_palette += 0 + ',' + p.palette() + ',' + '0xFFFFFFFF' + ',';
		}
		
		puts('========================================================================');
		puts('SpriteGroup spr(\n' +
			'\tarray<string>={' + sprite_set_name + '},\n'
			'\tarray<int>={' + layer_sub_layer + '},\n'
			'\tarray<float>={' + align_offset_rotation_scale + '},\n'
			'\tarray<uint>={' + colour_frame_palette + '});');
		puts('========================================================================');
	}
	
}