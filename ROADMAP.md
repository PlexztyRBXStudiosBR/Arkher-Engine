# Arkher Engine — Roadmap técnico

> Metas: fotorrealismo em qualquer celular · o melhor editor de terreno ·
> animação melhor que mocap humano · física de alto nível · 95% customizável.
>
> Este documento é o contrato de engenharia: cada pilar tem **diagnóstico**
> (o que o Godot 4.8 já tem), **plano** (o que a Arkher constrói) e **critério
> de saída** (como medimos que está pronto).

---

## Diagnóstico honesto

O Godot 4.8-dev já entrega: renderer Vulkan Forward+ com SDFGI e VSM,
Jolt Physics, shader language rico, GDExtension, mobile-first (GLES3/Compat).
O que **falta** para a meta da Arkher, ponto a ponto:

1. **Fotorrealismo em "qualquer celular"** — nenhum engine atual roda
   fotorrealismo de PC em um Galaxy A-series. A resposta técnica real não é
   "um modo de render", é **três tiers** (Ultra/High/Mobile) + **geometria
   virtualizada** + **GI por sondas** + **auto-escala de qualidade** que
   mantém 60fps dentro do orçamento térmico. É isso que vamos construir.
2. **Editor de terreno** — o Godot tem quase nada (terrain é plugin de
   terceiros, genérico). Chance real de sermos referência: quem constrói
   primeiro o editor de terreno **nativo, streaming, com penhascos SDF e
   erosão em tempo real**, ganha o nicho.
3. **Animação** — o Godot tem AnimationPlayer/BoneMap/IK básico (TwoBoneIK).
   Faltam motion matching, IK procedural de cadeia, root motion integrado e
   captura. "Melhor que exoesqueleto" = **qualidade acima do mocap**, que se
   alcança com motion matching + captura por **câmera do celular** (sem
   suíte, sem marcador) + retargeting.
4. **Física** — Jolt é top mundial em rígidos. Faltam: cloth/soft body de
   verdade, destruição, fluidos.
5. **Customização** — o modelo Godot (módulos C++ + GDExtension + resources)
   já resolve 95% do caminho; falta apenas garantir que **tudo** da Arkher
   siga esse padrão (regra de arquitetura, abaixo).

> Nota de realismo: "a melhor engine de todas" é uma corrida de anos com
> equipe dedicada. O roadmap abaixo foi recalibrado para o contexto real:
> **1 dev (solo, tempo integral) + o agente** escrevendo C++/shaders com
> review humano. Onde um item exigir 2 mãos simultâneas, ele vira 2 marcos.
> Cada marco tem demo testável — nada de "progresso invisível".

### Dispositivos de benchmark (o que "qualquer celular" significa aqui)

| Papel | Dispositivo | GPU | Por quê |
|-------|-------------|-----|---------|
| **Piso (gate)** | **Itel A70** (720p, 8GB) | Mali-G57 | O celular real de quem desenvolve — se rodar aqui, roda em 90% do mercado Android de entrada |
| Intermediário | (comprar um Redmi/Galaxy A a 3-4 meses do M2) | Adreno 610–640 | O "celular médio" de verdade |
| Topo | (qualquer flagship à mão) | Adreno 7xx / Xclipse | Referência de qualidade máxima |

**Regra:** nenhuma feature de render entra no trunk se derrubar o piso
(A70) abaixo de 30fps no tier Mobile.

---

## Pilar 1 — Fotorrealismo em qualquer celular

### 1.1 Auto-qualidade (base de tudo) — ✅ entregue (M0)
- `ArkherAutoQuality` (C++, módulo `arkher`): mede FPS (EMA) + modo de
  economia de processador, recomenda `scale` + tier via sinal
  `quality_changed`.
- `ArkherQualityDirector` (GDScript, addon `arkher_quality` v0.2): aplica a
  recomendação de verdade — **resolução dinâmica com histerese + FSR/FSR2 +
  tiers** (sombras/SDFGI/SSAO/SSIL/glow/volumétrico/SSR/reflexos/MSAA) com
  overlay de debug. Roda no Godot estável E no fork, sem compilar.
- Cena de teste: `addons/arkher_quality/demo_quality.tscn`.
- **M2:** `ArkherQualityDirector` — consumido pelo engine: escala resolução
  dinâmica (render a 1/8–1/1 da resolução, upscale FSR), liga/desliga
  sombras/SDFGI/reflexos por tier, e **teto térmico** (no fork 4.8-dev o
  core não expõe bateria; o teto térmico entra via API da plataforma
  Android — trabalho do M2). Critério: o mesmo jogo mantém ≥55fps em
  3 phones de referência (baixo/médio/topo) por 10 min sem throttling.

### 1.2 Geometria virtualizada (o "Nanite")
- Formato: meshes convertidas para **clusters** (meshlets) com LOD por
  cobertura de tela, culling GPU em compute (frustum + occlusion por
  cluster), rasterização híbrida (GPU normal + software fallback em
  compute/fragment para LODs altíssimos).
- Etapas:
  1. (M3) Pipeline de conversão de asset → clusters + culling por cluster
     Forward (ganho imediato mesmo sem SDF: 10M+ tri em mobile).
  2. (M4) Tesselação de SDF e LOD virtual contínuo.
  3. (M5) Interação com terreno (Pilar 2) e destruição (Pilar 4).
- Alternativa pragmática: até o M4, **HLOD agressivo + GPU culling por
  cluster** cobre 80% do benefício com 20% do esforço.

### 1.3 Luz e GI mobile
- (M4) **GI por sondas de tela + radiance cache** (estilo Lumen) para
  desktop/Ultra; tier Mobile: **probe-based** (relight em 3 camadas:
  ambiente, céu, emissivo) — suficiente para "parece fotorreal" em 720p/1080p.
- SDFGI (já existe no Godot) virar opcional pesada; sombras: VSM (existe)
  + cascata por tier.
- (M5) Path tracing em compute para baking e "modo câmera" no editor.

### 1.4 Pós e upscaling
- TAAU (já existe), FSR (MIT — licença ok) como upscaler padrão;
- SSS, hair (anisotrópico), thin-film para pele/cabelo;
- Tone mapping ACES + grading via node graph (exists no Godot, polir).

**Critério de saída do pilar (M5):** demo "Arkher Photoreal" — 1 cena com
personagem, vegetação densa, penhasco, chuva — rodando **a olho** em:
(1) topo de linha a 60fps, (2) celular intermediário a 45–60fps em 1080p,
(3) celular barato a 30fps em 720p com tier Mobile — sem perder o "fotograma".

---

## Pilar 2 — O melhor editor de terreno

### Estado atual: v0.1 entregue (`addons/arkher_terrain`)
Heightmap + 4 materiais (splat por vértice), escultura (levantar/abaixar/
suavizar/achatar), pintura de materiais, ruído FBM, erosão hidráulica,
save/load PNG16, pincel visual no viewport 3D do editor, undo por stroke.

### Evolução (do addon GDScript para o módulo C++ `arkher.terrain`)
| Versão | Conteúdo | Marco |
|--------|----------|-------|
| v0.1 (agora) | Heightmap, escultura, 4 splats, ruído, erosão, save/load | M0 ✅ |
| v0.5 | Splats 16-bit, 8 materiais, import PNG16/EXR (Terragen/Gaea), mais brushes (cliff, noise, height paint), undo/redo completo | M2 |
| v1.0 | **Quadtree + streaming** (mapas de 100 km²), **penhascos por SDF** (cliffs infinitos em bordas e ravinas), LODs por cluster integrados ao Pilar 1, cavernas (3D noise + metaballs), rios/estradas com máscara, vegetação por instancing (GPUI) com rules de bioma | M3–M4 |
| v1.5 | Erosão em tempo real no viewport (Taffy-style), escultura GPU (compute), simbiose com a física (heightfield collider auto-atualizado) e com o animator (slope detection) | M5 |

**Critério de saída:** escultar um vale de 5 km² com penhascos e 6 biomas,
com streaming sem hitching em celular, em tempo menor que qualquer
concorrente (medido em benchmark público).

---

## Pilar 3 — Animator melhor que mocap

### 3.1 Runtime
| Versão | Conteúdo | Marco |
|--------|----------|-------|
| v0.5 | **Motion Matching** (feature list + GPU search, estilo NME) para humanoides; blend espacial; root motion nativo (locomotion sem bone-mapping manual) | M3 |
| v1.0 | **IK procedural**: FABRIK/CCD de cadeia, Two-Bone melhorado, reach/grab, foot-lock em terreno (usa heightmap do Pilar 2); "ProceduralMotion" = estado procedural que dirige o skeleton direto (sem animações) para idle, respiração, reação a dano | M4 |
| v1.5 | Hair/cloth: simulação GPU (partículas + constraints, estilo xHair) com LOD mobile | M5 |

### 3.2 "Melhor que exoesqueleto" = captura + retargeting
- (M3) **Arkher Capture**: ferramenta (web + desktop) que usa **câmera do
  celular** — pose estimation (MoveNet/MediaPipe) → skeleton estimado →
  **retargeting automático** para qualquer rig com bone-mapping assistido por
  nome (H-Anim/Mixamo padrão).
- (M4) Corretivas: foot contact fixado, smoothing temporal, blend com
  motion matching → o resultado **supera mocap por marcadores em jogos**
  (onde o target é performance, não filme).
- (M5) Refinamento por ML: denoising de pose + "inpainting" de frames.

**Critério de saída:** ator amador grava 30s com o celular; em <5 min o
clipe roda no personagem da demo com foot-lock perfeito em terreno irregular —
julgado cego contra take de mocap profissional.

---

## Pilar 4 — Física absurda de real

| Versão | Conteúdo | Marco |
|--------|----------|-------|
| v0.5 | Hardening do Jolt: heightfield collider auto do terreno, profiling mobile (ilhas de física), determinismo cross-platform | M3 |
| v1.0 | **Cloth/soft body PBD** (GPU, com LOD), **destruição** (fracture por voxel + debris que vira rígido, depois cluster do Pilar 1) | M4 |
| v1.5 | **Fluidos** (MPS/SPH simplificado, GPU) para água de cena/efeito; **veículo** com suspensão por raycast + torque | M5 |

**Critério de saída:** cena de demonstração "destruição de casa em terreno"
(200+ debris, chuva, carro) a 60fps no tier Ultra e 30fps no Mobile.

---

## Pilar 5 — 95% customizável (regra de arquitetura)

Regras que toda feature Arkher **deve** seguir (reviews de PR checam isso):

1. **Toda feature expõe API pública**: classe `GDREGISTER_CLASS` no módulo
   `arkher`, documentada em `doc_classes/*.xml`, usável de GDScript.
2. **Nada de estado oculto**: configuração via `Resource` (`.tres`) → o
   designer altera sem C++.
3. **Shader language**: features de render expostas como uniforms/nodes de
   shader, nunca hardcoded no pipeline.
4. **GDExtension-first**: qualquer subsystem pesado pode ser reescrito em
   Rust/C# sem tocar no core (o pipeline de import/export já suporta).
5. **Hot reload**: módulos de editor reinicializam sem restart.
6. **5% fixos** listados em [ARKHER.md](ARKHER.md) — PRs que toquem nesses
   arquivos exigem aprovação dupla.

---

## Marcos (milestones)

| Marco | Janela | Entregas | Gate (critério para seguir) |
|-------|--------|----------|-----------------------------|
| **M0** ✅ | agora | Fork 4.8-dev, identidade, módulo `arkher` v0.1 (ArkherAutoQuality), terrain v0.1 | Build do editor verde + terrain v0.1 demo |
| **M1** | sem 1–4 | Rebrand completo (binário/logo/strings "Arkher"), CI próprio, docs, **Alpha 0.1 pública** | Quem clona o repositório compila e vê "Arkher Engine" em < 1 dia — ✅ rebrand feito; CI "Arkher Builds" ativo (1º run em validação) |
| **M2** | sem 5–12 | Terrain v0.5 (16-bit, 8 splats, import EXR), QualityDirector C++ (teto térmico), **demo Photoreal-1** | Demo ≥55fps no A70 (piso) e no topo; QualityDirector já entregue em v0.2 (GDScript) |
| **M3** | mês 4–8 | Motion matching + root motion, Terrain v1.0 (streaming+penhascos), Jolt hardening, Arkher Capture v0.5 | Personagem corre por terreno de 5 km² com foot-lock, em celular |
| **M4** | mês 8–14 | Geometria virtualizada v1 (clusters + culling), GI por sondas, Cloth/destruição, Capture v1.0 | 10M tri + GI em 1080p em celular intermediário |
| **M5** | mês 14–24 | **Arkher 1.0**: jogo showcase fotorreal, fluidos, hair/cloth, capture 2.0 (ML) | Review público de terceiros: "roda em celular e parece console" |

### Plano de 90 dias (agora → M2)

1. **Semana 1** — M1: rebrand (binário `arkher`, logo, strings do editor), CI no fork, página de docs.
2. **Semana 2–3** — QualityDirector em C++ + FSR; medir nos phones da equipe.
3. **Semana 4** — Alpha 0.1 (builds + documentação de instalação).
4. **Semana 5–8** — Terrain v0.5 (16-bit, EXR, 8 splats) migrando do addon para C++.
5. **Semana 9–12** — Demo Photoreal-1: cena, personagem, 3 phones, relatório de perf público.

## Riscos (e mitigação)

| Risco | Mitigação |
|-------|-----------|
| Nanite-like é 12–18 meses de GPU shader puro | HLOD+cluster culling primeiro (M3) já entrega valor; SDF depois |
| Mobile fragmenta (Mali/Adreno/Apple, drivers) | Tier Mobile conservador + CI de perf em 3 phones reais desde M2 |
| Fork diverge do upstream (conflitos ao atualizar) | `modules/arkher` isolado, upstream via branch mensal, nada de patch em `core/` |
| Escopo infinito (5 pilares de uma vez) | Cada marco tem gate; pilar só avança com demo testável |
| "95% customizável" vira bagunça API | Regra 1–6 do Pilar 5 como checklist de PR |

## O que eu (agente) posso fazer a seguir neste repo

- M1: varredura de rebrand (strings "Godot" → "Arkher" no editor), CI.
- Terrain v0.5: migrar para C++ (`modules/arkher/terrain_*`), 16-bit, 8 splats.
- QualityDirector em C++ com resolução dinâmica real.
- Scaffolds de motion matching (estilo feature list) e capture (ferramenta web).
- Cada uma vira branch + PR testável. Diga qual atacar.
