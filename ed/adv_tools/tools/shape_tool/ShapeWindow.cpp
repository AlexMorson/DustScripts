#include "../../../../lib/ui3/elements/TileShapeGraphic.cpp"

const array<int> SHAPE_MAP = {
	 8, 16,  0,  0,
	 7, 15,  0,  0,
	11,  3, 20, 17,
	12,  4, 19, 18,
	10,  9,  1,  2,
	 6,  5, 13, 14
};

class ShapeWindow : Window
{

	private int _tile_shape = 0;
    private int _selection_index = 2;

	int tile_shape
	{
		get const { return _tile_shape; }
		set
		{
			if (value < 0 or value > 20) return;
			int index = SHAPE_MAP.find(value);
			cast<Button>(get_child(index)).selected = true;
		}
	}

	ShapeWindow(UI@ ui)
	{
		super(ui, "Shape", false);

		GridLayout@ grid = GridLayout(ui, 4);
		grid.row_spacing = 0;
		grid.column_spacing = 0;
		@layout = grid;

		ButtonGroup@ button_group = ButtonGroup(ui, false);
		button_group.select.on(EventCallback(on_button_select));

		for (int i=0; i<24; ++i)
		{
			int tile_shape = SHAPE_MAP[i];
			TileShapeGraphic@ t = TileShapeGraphic(ui, tile_shape);

			Button@ b = Button(ui, t);
			b.name = "" + i;
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

    void move_selection(int dx, int dy)
    {
        const int x = (_selection_index % 4) + dx;
        const int y = (_selection_index / 4) + dy;
        if (x < 0 or x > 3 or y < 0 or y > 5) return;

        const int index = 4 * y + x;
        cast<Button>(get_child(index)).selected = true;
    }
	
	// ///////////////////////////////////////////
	// Events
	// ///////////////////////////////////////////
	
	private void on_button_select(EventInfo@ event)
	{
		if (event.target is null) return;

		_selection_index = parseInt(event.target.name);
		_tile_shape = SHAPE_MAP[_selection_index];
	}

}
