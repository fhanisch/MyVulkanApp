#include <vulkan\vulkan.h>
#include "matrix.h"
#include "renderobject.h"

void initRenderObject(RenderObject *pObj, PipelineCreateInfo createInfo)
{
	pObj->pipelineCreateInfo = createInfo;
}