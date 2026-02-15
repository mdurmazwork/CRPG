#include "CRPG_AIController.h"
#include "CRPG_CharacterBase.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Kismet/GameplayStatics.h"

ACRPG_AIController::ACRPG_AIController()
{
	PrimaryActorTick.bCanEverTick = true;
	bIsCombatActive = false;
	CurrentTarget = nullptr;

	// 1. Algý Bileþenini Oluþtur 
	AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
	SetPerceptionComponent(*AIPerception);

	// 2. Görme Duyusunu Ayarla
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));

	SightConfig->SightRadius = 1500.0f; // Ne kadar uzaðý görüyor?
	SightConfig->LoseSightRadius = 1800.0f; // Görüþten çýkma mesafesi
	SightConfig->PeripheralVisionAngleDegrees = 90.0f; // Görüþ açýsý (Göz ucuyla görme)

	// Yaþýný doldurmuþ verileri ne zaman silsin?
	SightConfig->SetMaxAge(5.0f);

	// Kimleri algýlasýn? (Düþman, Dost, Nötr)
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

	// Duyuyu kaydet
	AIPerception->ConfigureSense(*SightConfig);
	AIPerception->SetDominantSense(SightConfig->GetSenseImplementation());
}

void ACRPG_AIController::BeginPlay()
{
	Super::BeginPlay();

	// Algý sistemi tetiklendiðinde çalýþacak fonksiyonu baðla
	if (AIPerception)
	{
		AIPerception->OnTargetPerceptionUpdated.AddDynamic(this, &ACRPG_AIController::OnTargetDetected);
	}
}

void ACRPG_AIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	// AI bedene girdiðinde yapýlacaklar (Init iþlemleri)
}

void ACRPG_AIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// Gerçek zamanlý takip gerekirse buraya eklenebilir.
	// C-RPG olduðu için genelde TurnManager üzerinden tetikleneceðiz.
}

void ACRPG_AIController::OnTargetDetected(AActor* Actor, FAIStimulus Stimulus)
{
	// Kendi kendimizi algýlamayalým
	if (!Actor || Actor == GetPawn()) return;

	// Sadece baþarýlý algýlamalarý iþle (Görüþten çýktýysa Stimulus.WasSuccessfullySensed false döner)
	if (Stimulus.WasSuccessfullySensed())
	{
		if (IsHostile(Actor))
		{
			// Eðer yeni bir tehditse veya mevcut hedef yoksa
			if (!CurrentTarget)
			{
				CurrentTarget = Actor;
				bIsCombatActive = true;

				UE_LOG(LogTemp, Error, TEXT("AI BRAIN: %s spotted HOSTILE %s! Engaging Combat Mode."), *GetPawn()->GetName(), *Actor->GetName());

				// TODO: Burada GameMode'a veya TurnManager'a "Savaþ Baþlat" sinyali gönderilecek.
			}
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("AI BRAIN: %s saw neutral/friendly %s. Ignoring."), *GetPawn()->GetName(), *Actor->GetName());
		}
	}
	else
	{
		// Hedef görüþten çýktý
		if (Actor == CurrentTarget)
		{
			UE_LOG(LogTemp, Warning, TEXT("AI BRAIN: %s lost sight of target."), *GetPawn()->GetName());
			// Takip etme mantýðý (Last Known Location) buraya eklenebilir.
		}
	}
}

bool ACRPG_AIController::IsHostile(AActor* TargetActor) const
{
	ACRPG_CharacterBase* MyChar = Cast<ACRPG_CharacterBase>(GetPawn());
	ACRPG_CharacterBase* OtherChar = Cast<ACRPG_CharacterBase>(TargetActor);

	if (MyChar && OtherChar)
	{
		// Eðer biz Düþmansak ve karþýdaki Oyuncuysa -> Saldýr
		if (MyChar->TeamID == ETeamType::Enemy && OtherChar->TeamID == ETeamType::Player)
		{
			return true;
		}

		// Eðer biz Oyuncuyuz (yoldaþ) ve karþýdaki Düþmansa -> Saldýr
		if (MyChar->TeamID == ETeamType::Player && OtherChar->TeamID == ETeamType::Enemy)
		{
			return true;
		}
	}

	return false;
}

void ACRPG_AIController::DecideBestAction()
{
	if (!GetPawn() || !CurrentTarget)
	{
		UE_LOG(LogTemp, Warning, TEXT("AI BRAIN: No pawn or no target. Skipping turn."));
		return;
	}

	// --- UTILITY AI MANTIÐI (BASÝT PUANLAMA) ---

	float DistToTarget = FVector::Dist(GetPawn()->GetActorLocation(), CurrentTarget->GetActorLocation());

	float Score_Attack = 0.0f;
	float Score_Move = 0.0f;
	float Score_Defend = 0.0f;

	// 1. Saldýrý Puaný
	// Menzil içindeyse puan artar
	if (DistToTarget < 800.0f) Score_Attack += 50.0f;
	if (DistToTarget < 200.0f) Score_Attack += 30.0f; // Çok yakýnda vurma ihtimali artar

	// 2. Hareket Puaný
	// Uzaktaysa yaklaþma isteði artar
	if (DistToTarget > 1000.0f) Score_Move += 60.0f;

	// 3. Savunma Puaný (Örnek)
	// Can azsa savunma artar (AttributeComponent lazým)

	// --- KARAR ANI ---
	UE_LOG(LogTemp, Log, TEXT("AI BRAIN DECISION SCORES: Attack: %.1f, Move: %.1f"), Score_Attack, Score_Move);

	if (Score_Attack > Score_Move)
	{
		UE_LOG(LogTemp, Error, TEXT("AI ACTION: ATTACKING target!"));
		// ActionComponent->ExecuteAbility(...)
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AI ACTION: MOVING towards target!"));
		// GridManager->MoveTo(...)
	}
}