const string EMBED_spr_icon_depth_tool = SPRITES_BASE + 'icon_depth_tool.png';

class DepthTool : Tool
{
	DepthTool(AdvToolScript@ script)
	{
		super(script, 'Depth Tool');
		
		init_shortcut_key(VK::D);
	}

	void build_sprites(message@ msg) override
	{
		build_sprite(msg, 'icon_depth_tool');
	}

	void create(ToolGroup@ group) override
	{
		Tool::create(group);
		
		set_icon(SPRITE_SET, 'icon_depth_tool', 33, 33);
	}

	void step_impl() override
	{
		if (script.mouse.left_press and not script.space and script.mouse_in_scene)
		{
			for (int layer = 20; layer > 5; --layer)
			{
				if (not script.editor.get_layer_visible(layer))
					continue;

				const int tx = floor_int(script.g.mouse_x_world(0, layer) / 48);
				const int ty = floor_int(script.g.mouse_y_world(0, layer) / 48);
				tileinfo@ tile = script.g.get_tile(tx, ty, layer);

				if (tile !is null and tile.solid())
				{
					move_to_front(tx, ty, layer);
					break;
				}
			}
		}
	}

	void on_select_impl() override
	{
		script.editor.hide_panels_gui(true);
	}

	void on_deselect_impl() override
	{
		script.editor.hide_panels_gui(false);
	}

	private void move_to_front(int tx, int ty, int layer)
	{
		HashMap seen;
		array<Node> todo;

		Key@ start_key = Key(tx, ty);
		tileinfo@ start_tile = script.g.get_tile(tx, ty, layer);
		seen.add(start_key, start_tile);
		todo.insertLast(Node(start_key, start_tile));

		while (todo.size() > 0)
		{
			Node@ next = todo[todo.size() - 1];
			todo.removeLast();

			Key@ pos = next.key;
			tileinfo@ tile = next.value;

			tileinfo@ tile_top = script.g.get_tile(pos.x, pos.y - 1, layer);
			tileinfo@ tile_bottom = script.g.get_tile(pos.x, pos.y + 1, layer);
			tileinfo@ tile_left = script.g.get_tile(pos.x - 1, pos.y, layer);
			tileinfo@ tile_right = script.g.get_tile(pos.x + 1, pos.y, layer);

			bool same_top = same_tile_type(tile, tile_top);
			bool same_bottom = same_tile_type(tile, tile_bottom);
			bool same_left = same_tile_type(tile, tile_left);
			bool same_right = same_tile_type(tile, tile_right);

			if (same_top)
			{
				Key@ pos_top = Key(pos.x, pos.y - 1);
				if (seen.get(pos_top) is null)
				{
					seen.add(pos_top, tile_top);
					todo.insertLast(Node(pos_top, tile_top));
				}
			}

			if (same_bottom)
			{
				Key@ pos_bottom = Key(pos.x, pos.y + 1);
				if (seen.get(pos_bottom) is null)
				{
					seen.add(pos_bottom, tile_bottom);
					todo.insertLast(Node(pos_bottom, tile_bottom));
				}
			}

			if (same_left)
			{
				Key@ pos_left = Key(pos.x - 1, pos.y);
				if (seen.get(pos_left) is null)
				{
					seen.add(pos_left, tile_left);
					todo.insertLast(Node(pos_left, tile_left));
				}
			}

			if (same_right)
			{
				Key@ pos_right = Key(pos.x + 1, pos.y);
				if (seen.get(pos_right) is null)
				{
					seen.add(pos_right, tile_right);
					todo.insertLast(Node(pos_right, tile_right));
				}
			}
		}

		HashMapIterator it(seen);
		Node@ next;
		while ((@next = it.next()) !is null)
		{
			Key@ pos = next.key;
			script.g.set_tile(pos.x, pos.y, layer, next.value, true);
		}
	}
}

bool same_tile_type(tileinfo@ x, tileinfo@ y)
{
	return (
		x.solid() == y.solid() and
		x.sprite_set() == y.sprite_set() and
		x.sprite_tile() == y.sprite_tile() and
		x.sprite_palette() == y.sprite_palette()
	);
}

class HashMap
{
	uint size = 0;
	uint buckets = 8;
	array<array<Node>>@ table = array<array<Node>>(buckets);

	tileinfo@ get(int x, int y) const
	{
		return get(Key(x, y));
	}

	tileinfo@ get(Key@ key) const
	{
		uint bucket = key.hash % buckets;
		for (uint i=0; i<table[bucket].size(); ++i)
		{
			if (table[bucket][i].key == key)
			{
				return table[bucket][i].value;
			}
		}
		return null;
	}

	void add(Key@ key, tileinfo@ value)
	{
		uint bucket = key.hash % buckets;

		// Overwrite if it already exists
		for (uint i=0; i<table[bucket].size(); ++i)
		{
			if (table[bucket][i].key == key)
			{
				@table[bucket][i].value = value;
				return;
			}
		}

		// Resize if neccessary
		if (size >= uint(0.7 * buckets))
		{
			size = 0;
			buckets *= 2;
			bucket = key.hash % buckets;
			array<array<Node>>@ old_table = table;
			@table = array<array<Node>>(buckets);
			for (uint i=0; i<buckets/2; ++i)
			{
				for (uint j=0; j<old_table[i].size(); ++j)
				{
					add(old_table[i][j].key, old_table[i][j].value);
				}
			}
		}

		// Add it
		++size;
		table[bucket].insertLast(Node(key, value));
	}

	void remove(Key@ key)
	{
		int bucket = key.hash % buckets;
		for (uint i=0; i<table[bucket].size(); ++i)
		{
			if (table[bucket][i].key == key)
			{
				table[bucket].removeAt(i);
				--size;
				return;
			}
		}
	}
}

class HashMapIterator
{
	HashMap@ m;
	uint bucket = 0;
	uint element = 0;

	HashMapIterator(HashMap@ m)
	{
		@this.m = m;
	}

	void reset()
	{
		bucket = 0;
		element = 0;
	}

	Node@ next()
	{
		while (bucket < m.buckets)
		{
			while (element < m.table[bucket].size())
			{
				return m.table[bucket][element++];
			}
			++bucket;
			element = 0;
		}
		return null;
	}
}

class Key
{
	int x;
	int y;
	int hash;

	Key(int x, int y)
	{
		this.x = x;
		this.y = y;
		this.hash = 40009 * (20000 + x) + (20000 + y);
	}

	bool opEquals(Key@ &in other)
	{
		return x == other.x and y == other.y;
	}
}

class Node
{
	Key@ key;
	tileinfo@ value;

	Node() {}

	Node(Key@ key, tileinfo@ value)
	{
		@this.key = key;
		@this.value = value;
	}
}
