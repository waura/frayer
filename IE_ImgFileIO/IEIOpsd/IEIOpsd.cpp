#include "IEIOpsd.h"

#ifdef _MANAGED
#pragma managed(push, off)
#endif

HMODULE g_hModule;
bool g_isInterlaced;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	g_hModule = hModule;
    return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

IEIO_EXPORT IEMODULE_ID GetIEModule_ID()
{
	return IEMODULE_ID::IMG_FILE_IO;
}

IEIO_EXPORT void IEIO_GetFilter(char filter[], size_t buf_size)
{
	strcpy_s(filter, buf_size, "PhotoShop(*.psd)|*.psd;*.PSD");
}

void free_psd_layer_data(int nLayer, PsdLayerData* pLayerData)
{
	int i;
	for(i=0; i<nLayer; i++){
		if(pLayerData[i].psd_channels){
			free(pLayerData[i].psd_channels);
		}
		if(pLayerData[i].name){
			free(pLayerData[i].name);
		}
	}
	free(pLayerData);
}

void free_psd_layer_data_list(PsdLayerData_List& psd_layer_data_list)
{
	PsdLayerData_List::iterator itr = psd_layer_data_list.begin();
	for(; itr != psd_layer_data_list.end(); itr++){
		if(*itr){
			if((*itr)->psd_channels){
				free((*itr)->psd_channels);
			}
			if((*itr)->name){
				free((*itr)->name);
			}
			free((*itr));
		}
	}
}
