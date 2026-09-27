# Arkher Terrain (addon) — v0.1

Editor de terreno da **Arkher Engine**. v0.1 funcional em GDScript; a versão
1.0 migrará para o módulo C++ `modules/arkher/` (streaming, penhascos SDF,
splats 16-bit).

## Como usar

1. No seu projeto, copie esta pasta para `res://addons/`.
2. Project → Project Settings → Plugins → ative **ArkherTerrain**.
3. Na 3D Scene: *Add Node* → **ArkherTerrain** (icône de montanha).
4. Selecione o nodo na Scene Tree — o painel **Arkher Terrain** abre no dock
   esquerdo.
5. No viewport 3D, clique com o botão **ESQUERDO** em área vazia perto do
   terreno e arraste para esculpir. (Não segure no gizmo de mover.)

### Ferramentas
| Botão | Ação |
|-------|------|
| Levantar / Abaixar | Esculpe o heightmap |
| Suavizar | Suaviza o relevo sob o pincel |
| Achatar | Aplainar na altura onde clicou (clique 1x para definir o nível) |
| Pint. A–D | Pinta o peso do material A/B/C/D (canais R/G/B/A) |

### Materiais
Atribua uma textura de albedo para cada material (A..D) no painel e ajuste
Roughness/Metallic. A textura do material A cobre o terreno por padrão.

### Gerar / Erosão
- **Gerar (ruído FBM)**: relevo procedural (FastNoiseLite, Perlin octaves).
- **Erodir (hidráulica)**: partículas de água escavam vales e depositam
  sedimento — v0.1 simples; a v1.0 trará Taffy-style em tempo real.

### Dados
Heightmap (16-bit) e pesos (RGBA8) são salvos em
`<pasta_da_cena>/arkher_terrain_<nome>/{heights,weights}.png` com
**Salvar dados** e reabertos em **Carregar dados**. O undo do editor cobre
cada stroke (Ctrl+Z).

### Limitações conhecidas da v0.1
- Splat por vértice (resolução dos pesos = resolução do grid) → v0.5:
  splatmap 16-bit separada.
- Mesh única (sem streaming/LOD) → v1.0: quadtree + clusters.
- Edits não sobrevivem a `Ctrl+Z` de mudanças de cena inteiras — salve os
  dados com frequência.
