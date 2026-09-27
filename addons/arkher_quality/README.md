# Arkher Quality (addon) — v0.3

**ArkherQualityDirector** — a base do pilar "fotorreal em qualquer celular".
Pure GDScript: **não precisa compilar a engine** — funciona no Godot estável
(4.3/4.4+) E no fork Arkher (4.8-dev) — com tolerância de versão nas APIs
que o fork renomeou (`shadow`/`distance_fade_shadow`, `msaa_3d`,
`reflected_light_source`).

## Demo Photoreal-1 (M2)

`demo_photoreal_1.tscn` — cena de teste do pipeline fotorreal: céu
procedural (golden hour), AgX, SDFGI, SSAO, SSIL, glow, fog volumétrico,
SSR e reflexos, com o QualityDirector ajustando tudo sozinho. Abra e dê
**Play**: o overlay no canto mostra fps/escala/tier em tempo real.

## O que ele faz

1. **Resolução dinâmica** com histerese: mede o FPS real; se ficar abaixo do
   alvo por `down_after_seconds`, baixa a escala de render (`step_down`);
   se ficar acima por `up_after_seconds`, sobe (`step_up`). Escala fica entre
   `min_scale` e `max_scale` (padrão 0.4..1.0).
2. **Upscale com FSR/FSR2** quando o engine oferece (4.4+ e fork); senão,
   upscale bilinear.
3. **Tiers de qualidade** (0 low / 1 med / 2 high) derivados da escala —
   ligam/desligam: sombras (por luz, com distância máx.), SDFGI, SSAO, SSIL,
   glow, fog volumétrico, SSR, reflexos, MSAA. Tudo configurável por tier
   no inspector (arrays `[low, medium, high]`).
4. **Overlay de debug** (canto superior esquerdo): `ARKHER fps | scale |
   tier | alvo` — essencial para ver a auto-quality agindo no celular.

## Como usar

1. Ative o plugin **ArkherQuality** (Project Settings → Plugins).
2. Adicione um nodo **ArkherQualityDirector** na cena raiz.
3. Selecione o `WorldEnvironment` do jogo na propriedade `world_environment`.
4. Ajuste `target_fps` (ex.: 60), `min_scale` (ex.: 0.4).
5. Pronto. Sinais: `scale_changed(scale)`, `tier_changed(tier)`,
   `quality_changed(scale, tier)`.

## Teste rápido no PC

Abra `addons/arkher_quality/demo_quality.tscn` → botão **Play**.
Para ver a auto-quality trabalhar: no project settings, reduza o FPS do
editor (Editor → Settings → Interface → ... ou use `OS.set_low_processor_usage_mode(true)`
num script temporário), ou baixe o `target_fps` para 20 — a escala vai
subir; suba para 120 — ela cai.

## Teste no Itel A70 (dispositivo de referência)

1. Godot → Editor → Manage Export Templates → baixe os templates.
2. Export → Android: crie/insira um keystore de debug, marque `one_click_deploy`.
3. Export para `res://demo.apk`, instale no A70 (USB debugging).
4. Rode a cena `demo_quality.tscn` — o overlay mostra fps/scale/tier em
   tempo real no celular.
5. **Critério do tier Mobile (ROADMAP M2):** a demo segura ≥55fps no A70
   com tier LOW/MED e escala entre 0.4 e 0.7 (720p nativo).

## Director nativo (C++, módulo `arkher` — M2)

A versão C++ **`ArkherQualityDirector`** (classe nativa, aparece no editor
quando a engine Arkher está compilada com o módulo `arkher`) adiciona:

- **Teto térmico (thermal model)**: `thermal_state` (0..1) sobe com a carga
  sustentada de render e desce quando o FPS atinge o alvo. Acima de
  `thermal_warning_threshold`, a escala máxima efetiva cai (até 50% de
  `max_scale` em calor total) e o sinal `thermal_warning` dispara.
  Este fork não expõe sensor de temperatura/bateria no core (como o Godot
  4.x), então o modelo é **portátil**: calibre `thermal_heat_rate` por
  dispositivo — comece conservador no Itel A70 e afrouxe em phones
  intermediários. Quando o renderer Arkher expuser telemetria real
  (M4), o modelo passa a ler o sensor de verdade.
- Mesma lógica de histerese + FSR/FSR2 + tiers (sombras, SDFGI, SSAO, SSIL,
  glow, volumétrico, SSR, reflexos, MSAA por tier).
- Sinais: `scale_changed`, `tier_changed`, `quality_changed`,
  `thermal_warning`.

Uso: adicione um nodo `ArkherQualityDirector` (só existe no build Arkher —
no Godot estável use a versão GDScript deste addon, que é a mesma lógica).

## Limitações (próximos passos)

- 1 `WorldEnvironment` por director; cenas multi-ambiente: lista de
  ambientes (M2.5).
- Presets como `Resource` salvos no projeto (M2.5).
- Leitura real de sensor térmico/bateria via GDExtension no Android (M4).
