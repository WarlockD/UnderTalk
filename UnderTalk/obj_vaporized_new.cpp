#include "obj_vaporized_new.h"

obj_vaporized_new::obj_vaporized_new(const std::string& str) {
	int ch = 0;
	int pos = 0;
	int ww = 0;
	int wd = 0;
	int spec = 0;
	while (ch != '}' && ch != '~') {
		ch = (pos + 1) < str.length() ? str[pos + 1] : 0;
		if (ch >= 84 && ch <= 121) {
			for (int i = 0; i < (ch - 85); i++, ww+=2); // I think we can do this with math?
		}
		if (ch >= 39 && ch <= 82) {
			if (wd > 120 && spec == 0) {
				GSprite sprite(191);

			}
		}
	}
	for (size_t i = 0; i < str.length(); i++) {
	
		while()
	}
}
void obj_vaporized_new::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	states.transform *= getTransform();
	for (auto& s : _sprites) target.draw(s, states);
}