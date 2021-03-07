#ifndef INCLUDE_GAME_OPTIONS_H_
#define INCLUDE_GAME_OPTIONS_H_

#include "enums.h"

namespace Tiles
{


class GameOptions
{
private:
	AceFreeingMethod _aceFreeingMethod;

public:
	GameOptions();

	AceFreeingMethod getAceFreeingMethod() const { return _aceFreeingMethod; }

};

} // namespace Tiles





#endif /* INCLUDE_GAME_OPTIONS_H_ */
