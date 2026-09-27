# Arkher Engine

> **Este repositório é a Arkher Engine** — uma engine de jogos derivada do
> [Godot Engine](https://godotengine.org) (MIT), mantida pela **PlexztyRBX Studios**.
> O objetivo: jogos **ultra-realistas e fotorrealistas rodando em qualquer celular**,
> com o melhor editor de terreno, animação melhor que captura por motion-capture e
> física de alto nível — e **95% customizável**.

- 📋 **Plano técnico e marcos** → [ROADMAP.md](ROADMAP.md)
- 🌄 **Editor de terreno (já funcional)** → [addons/arkher_terrain/](addons/arkher_terrain/README.md)
- ⚙️ **Primeiro módulo C++ da engine** → [modules/arkher/](modules/arkher/)
- 🔧 **O restante da árvore de código é o Godot upstream** (4.8-dev) — o motor base.

---

## O que a Arkher é (e o que ela ainda não é)

A Arkher **não** parte do zero: ela herda do Godot um motor maduro — cena/node,
render Vulkan (Compatibility + Forward+ + Mobile via GLES), Jolt Physics,
GDScript, GDExtension (C#/Rust/C++/Python), exportação para Android, iOS, Web,
Windows, Linux, macOS, e o editor completo.

Sobre essa base, a Arkher constrói seus **5 pilares de diferenciação**:

| # | Pilar | Meta |
|---|-------|------|
| 1 | **Fotorrealismo em qualquer celular** | Geometria virtualizada, GI de alta fidelidade com tiers de qualidade, upscaling, e auto-escala de performance (começando pelo módulo `ArkherAutoQuality`: FPS + modo de economia de energia → escala de render recomendada) |
| 2 | **O melhor editor de terreno** | Heightmap → quadtree streaming, splat 16-bit, penhascos por SDF, erosão, cavernas, vegetação (v0.1 já funcional em `addons/arkher_terrain`) |
| 3 | **Animator melhor que mocap** | Motion matching GPU, IK procedural, root motion, e **captura de animação pela câmera do celular** (retargeting automático para qualquer rig) |
| 4 | **Física absurda de real** | Jolt como base + soft body/cloth, destruição, fluidos, veículos |
| 5 | **95% customizável** | Tudo exposto via API C++, GDScript, GDExtension e módulos; dados como recursos; o editor em si é extensível |

### Os 5% que NÃO podem ser mexidos (e por quê)

| Bloqueado | Motivo |
|-----------|--------|
| 1. Licença MIT e créditos ao Godot/Fundação | Exigência legal — sem isso a engine não pode existir |
| 2. Núcleo de memória e segurança de objetos (`Object`, `Ref`, alocação) | Estabilidade do motor inteiro; tocar aqui = bug impossível de rastrear |
| 3. Formato de projeto/binário e numeração de versão | Compatibilidade com os jogos feitos na Arkher |
| 4. Sandbox de segurança de scripts | Proteção do jogador (scripts de terceiros) |
| 5. Pipeline de CI / verificação de licença | Garante que nenhum build vaze sem validar o que está acima |

Tudo o mais — renderer, física, animação, ferramentas, UI, pipeline de assets,
shaders — pode ser modificado, substituído ou estendido.

---

## Estrutura do repositório

```
Arkher-Engine/
├── modules/arkher/            # ⭐ MÓDULO C++ DA ARKHER (compilado dentro da engine)
│   └── arkher_auto_quality.*  #    - auto-escala de qualidade ("roda em qualquer celular")
├── addons/arkher_terrain/     # ⭐ EDITOR DE TERRENO v0.1 (plugin GDScript, funciona já)
├── ARKHER.md                  # este arquivo
├── ROADMAP.md                 # plano técnico por pilar + marcos M0–M6
├── version.py                 # identidade: "Arkher Engine"
└── ...                        # restante = Godot Engine upstream (MIT)
```

Convenções:
- **Algo que precisa estar DENTRO da engine** (API de runtime, performance crítica,
  integração profunda com o renderer) → `modules/arkher/` (C++, módulo Godot).
- **Algo que é ferramenta de editor ou protótipo** → `addons/` (GDScript, iterável em minutos).
- Ao migrar um addon para o módulo C++, mantemos o mesmo nome de classe pública
  para não quebrar jogos.

## Como compilar

Requisitos: Python 3.11+, scons, GCC/Clang (Linux), Xcode (macOS), MSVC (Windows).

```bash
# Editor (para desenvolver a engine + jogos nela)
scons platform=linux target=editor dev_build=yes

# Template de exportação (para empacotar jogos)
scons platform=android target=template_release arch=arm64
```

O módulo `arkher` e todos os addons compilam sozinhos — módulos Godot em
`modules/` são detectados automaticamente pelo build.

> **Build rápido para validar:** em máquina com poucos cores,
> `scons platform=linux target=editor_debug dev_build=yes -j$(nproc)` leva horas.
> Use CI (GitHub Actions) para builds completos.

## Licença e identidade

- Código: **MIT** — mesmo do Godot. `AUTHORS.md`, `COPYRIGHT.txt` e `LICENSE.txt`
  permanecem intactos (obrigação legal).
- A Godot Foundation pede que forks **não** se passem por "Godot" em marketing:
  usamos **Arkher Engine** em todo o lugar visível ao usuário, mantendo o crédito
  "derivada do Godot Engine".
- O `short_name` do build continua `godot` por enquanto por segurança de tooling;
  a renomeação completa (binário, logo, strings) é o marco **M1** do ROADMAP.

## Status

| Item | Estado |
|------|--------|
| Fork do Godot 4.8-dev | ✅ |
| Identidade "Arkher Engine" no banner | ✅ |
| Módulo C++ `arkher` (ArkherAutoQuality) | ✅ v0.1 |
| Editor de terreno `arkher_terrain` | ✅ v0.1 (escultura, 4 materiais, ruído, erosão) |
| QualityDirector `arkher_quality` | ✅ v0.3 (resolução dinâmica + FSR/FSR2 + tiers, GDScript, roda sem compilar, compat. Godot estável E fork) |
| QualityDirector C++ `ArkherQualityDirector` (M2) | ✅ v1 no módulo `arkher` (teto térmico + histerese + FSR2 + per-tier + sinais) |
| Demo Photoreal-1 (M2) | ✅ `addons/arkher_quality/demo_photoreal_1.tscn` (golden hour, AgX, SDFGI, SSAO, SSIL, glow, fog, SSR, reflexos) |
| Rebrand "Arkher Engine" (M1) | ✅ short_name, ícones do editor, logo, About, banner, `short_name` do build |
| CI do editor (M1) | ✅ workflow "Arkher Builds" → binário Linux com artefato (a validar no primeiro run) |
| Geometria virtualizada / GI mobile / motion matching | 🔜 ver ROADMAP.md |

### Dispositivo de referência (gate mobile)

**Itel A70** (Unisoc T606, Mali-G57, 720p) = piso do tier Mobile: o que não
rodar bem no A70 não entra na Arkher. Ver ROADMAP.md → "Dispositivos de
benchmark".
