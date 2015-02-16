template<class... CMixins>
class CX : public CMixins...
{
	public:
	void constructor(const CMixins&... mixins) : CMixins(mixins)... { }

	virtual void ~CX();

};
	
