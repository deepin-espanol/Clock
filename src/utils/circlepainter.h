#ifndef CIRCLEPAINTER_H
#define CIRCLEPAINTER_H

class QPainter;
class QPalette;
class QRect;

/**
 * @brief Renders a circle, might be animated.
 */
class CirclePainter
{
public:
	CirclePainter();

	/**
	 * @brief Render part of a circle (counterclockwise)
	 * @param p Source painter.
	 * @param palette Palette used for the colors.
	 * @param r Rect in which the circle has to be rendered.
	 * @param percent Part of the circle that has to be filled.
	 */
	void render(QPainter &p, const QPalette &palette, const QRect &r, const float percent);
	/**
	 * @brief Render circle, meant for animation.
	 * @param p Source painter.
	 * @param palette Palette used for the colors.
	 * @param r Rect in which the circle has to be paint.
	 */
	//Each time it is called, the size and anlge change, each painting then generates an "animation".
	void render(QPainter &p, const QPalette &palette, const QRect &r);
	/**
	 * @brief Reset the internal data used for the "animation".
	 */
	void reset();

private:
	//Used only to "animate".
	//Position of the part that has to be paint.
	float m_angle = 0;
	//Size filled with color.
	float m_size = 6;
};

#endif // CIRCLEPAINTER_H
