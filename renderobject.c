#include "MyVulkanEngine.h"
#include "matrix.h"
#include "renderobject.h"

void initRenderObject(RenderObject *pObj, PipelineCreateInfo createInfo)
{
	pObj->pipelineCreateInfo = createInfo;
}