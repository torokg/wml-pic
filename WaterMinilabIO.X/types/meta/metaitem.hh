#ifndef TYPES_META_METAITEM_HH
# define TYPES_META_METAITEM_HH
namespace types::meta
{

struct metaitem
{
	metaitem() = delete;
	metaitem(const metaitem&) = delete;
	metaitem(metaitem&&) = delete;
	metaitem(const metaitem&&) = delete;
	metaitem& operator=(const metaitem&) = delete;
	metaitem& operator=(metaitem&&) = delete;
	metaitem& operator=(const metaitem&&) = delete;
};

}
#endif
