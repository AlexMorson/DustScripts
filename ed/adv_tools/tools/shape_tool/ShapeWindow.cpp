#include "../../../../lib/ui3/elements/TileShapeGraphic.cpp"

class ShapeWindow : Window
{

	private int _tile_shape = 0;

	int tile_shape { get const { return _tile_shape; } }

	ShapeWindow(UI@ ui)
	{
		super(ui, "Shape", false);

		GridLayout@ grid = GridLayout(ui, 4);
		grid.row_spacing = 0;
		grid.column_spacing = 0;
		@layout = grid;

		ButtonGroup@ button_group = ButtonGroup(ui);
		button_group.select.on(EventCallback(on_button_select));

		const array<int> tile_map = {
			 8, 16,  0,  0,
			 7, 15,  0,  0,
			11,  3, 20, 17,
			12,  4, 19, 18,
			10,  9,  1,  2,
			 6,  5, 13, 14
		};

		for (int i=0; i<24; ++i)
		{
			int tile_shape = tile_map[i];
			TileShapeGraphic@ t = TileShapeGraphic(ui, tile_shape);

			Button@ b = Button(ui, t);
			b.name = "" + tile_shape;
			b.width = 48;
			b.height = 48;
			b.selectable = true;

			if (i == 2) 
				b.selected = true;

			button_group.add(b);
			add_child(b);
		}

		fit_to_contents(true);
	}
	
	// ///////////////////////////////////////////
	// Events
	// ///////////////////////////////////////////
	
	private void on_button_select(EventInfo@ event)
	{
		if (event.target is null) return;

		_tile_shape = parseInt(event.target.name);
	}

}
